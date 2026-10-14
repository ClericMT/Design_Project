// Server 2020
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>

// Local NativeMsgPass between server (LocalController.c) and client (LocalSensor.c)
// Derived from Lab5 NativeMsgPass-Client.c

// Local attach point (on same node) for client/server located at <hostname>/dev/name/local/LocalController"
#define ATTACH_POINT "LocalController"  // This must be the same name that is used for the client.

#define BUF_SIZE 300

typedef union {	  			// This replaced the standard:  union sigval
	union{
		_Uint32t sival_int;
		void *sival_ptr;	// This has a different size in 32-bit and 64-bit systems
	};
	_Uint32t dummy[4]; 		// Hence, we need this dummy variable to create space
}_mysigval;

typedef struct _Mypulse {   // This replaced the standard:  typedef struct _pulse msg_header_t;
   _Uint16t type;
   _Uint16t subtype;
   _Int8t code;
   _Uint8t zero[3];         // Same padding that is used in standard _pulse struct
   _mysigval value;
   _Uint8t zero2[2];		// Extra padding to ensure alignment access.
   _Int32t scoid;
} msg_header_t;


// This is where pedestrian/vehicle detection data is stored
typedef struct {
   int NS_Vehicle;          // north/south vehicle detected
   int EW_Vehicle;          // east/west vehicle detected
   int NS_Ped;              // north/south ped. crossing request
   int EW_Ped;              // east/west ped. crossing request
} SensorData;

typedef struct {
    msg_header_t hdr;
    int ClientID;
    SensorData data;
} SensorMessage;

typedef struct {
   msg_header_t hdr;   // Custom header
   char buf[BUF_SIZE]; // Message to send back to send back to other thread
} my_reply;

// prototypes
int server();

int main(int argc, char *argv[])
{
	printf("Server running\n");

    int ret = 0;
    ret = server(); // Run actual server code

	printf("Main (Server) Terminated....\n");
	return ret;
}



/*** Process received sensor data ***/
void handleSensorData(const SensorData* data)
{
    printf("\nReceived sensor data:\n");
    printf("  NS Vehicle: %d\n", data->NS_Vehicle);
    printf("  EW Vehicle: %d\n", data->EW_Vehicle);
    printf("  NS Pedestrian: %d\n", data->NS_Ped);
    printf("  EW Pedestrian: %d\n", data->EW_Ped);

    // TODO: Update controller's sensor state
}


/*** Handle QNX system pulses ***/
void handlePulse(const struct _pulse* pulse)
{
    switch (pulse->code)
    {
        case _PULSE_CODE_DISCONNECT:
            ConnectDetach(pulse->scoid);
            break;

        default:
            break;
    }
}


/*** Server code ***/
int server(void)
{
    name_attach_t* attach;
    SensorMessage sensorMessage = {0};
    my_reply replymsg = {0};

    int rcvid;
    int msgnum = 0;

    // Initialise reply
    replymsg.hdr.type = 0x01;
    replymsg.hdr.subtype = 0x00;

    // Create named communication channel
    attach = name_attach(NULL, ATTACH_POINT, 0);

    if (attach == NULL)
    {
        perror("name_attach");
        return EXIT_FAILURE;
    }

    printf("Server listening on: %s\n", ATTACH_POINT);

    // Main receive loop
    while (1)
    {
        rcvid = MsgReceive(
            attach->chid,
            &sensorMessage,
            sizeof(sensorMessage),
            NULL
        );

        // Receive error
        if (rcvid == -1)
        {
            perror("MsgReceive");
            break;
        }

        // QNX system pulse
        if (rcvid == 0)
        {
            handlePulse((const struct _pulse*)&sensorMessage);
            continue;
        }

        // QNX connection request
        if (sensorMessage.hdr.type == _IO_CONNECT)
        {
            MsgReply(rcvid, EOK, NULL, 0);
            continue;
        }

        // Unsupported QNX I/O message
        if (sensorMessage.hdr.type > _IO_BASE &&
            sensorMessage.hdr.type <= _IO_MAX)
        {
            MsgError(rcvid, ENOSYS);
            continue;
        }

        // Validate application message type
        if (sensorMessage.hdr.type != 0x22)
        {
            MsgError(rcvid, ENOSYS);
            continue;
        }

        // Process sensor data
        handleSensorData(&sensorMessage.data);

        // Acknowledge message
        msgnum++;
        snprintf(replymsg.buf, sizeof(replymsg.buf),
                 "Message %d received", msgnum);

        MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
    }

    // Cleanup
    name_detach(attach, 0);

    return EXIT_SUCCESS;
}

