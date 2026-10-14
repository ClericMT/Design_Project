// Client 2020
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>

// Local NativeMsgPass between server (LocalController.c) and client (LocalSensor.c)
// Derived from Lab5 NativeMsgPass-Client.c
// Assuming LocalSensor and LocalController are on the same node

// Define where the "local" attach point is located
#define LOCAL_ATTACH_POINT "LocalController"

#define BUF_SIZE 100

typedef union {
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

typedef struct {
    int NS_Vehicle;
    int EW_Vehicle;
    int NS_Ped;
    int EW_Ped;
} SensorData;

typedef struct {
    msg_header_t hdr;
    int ClientID;
    SensorData data;
} SensorMessage;

// This struct stores reply msg sent back to LocalController process
typedef struct {
   msg_header_t hdr;   // Custom header
   char buf[BUF_SIZE]; // Message to send back to send back to other thread
} my_reply;

// prototype
int client(char *sname);

int main(int argc, char *argv[])
{
	printf("_mysigval size= %d\n", sizeof(_mysigval));

	printf("header size= %d\n", sizeof(msg_header_t));
	printf("SensorData size= %d\n", sizeof(SensorData));
	printf("my_reply size= %d\n", sizeof(my_reply));

	printf("This is A Client running\n");

    int ret = 0;

    ret = client(LOCAL_ATTACH_POINT);  // Init. client code using LocalController attach point

	printf("Main (client) Terminated....\n");
	return ret;
}

/*** Client code ***/
int client(char *sname)
{
    SensorData sensorData = {0,1,2,3};
    SensorMessage sensorMessage = {0};

    sensorMessage.hdr.type = 0x22;
    sensorMessage.hdr.subtype = 0x00;
    sensorMessage.ClientID = 600;

    my_reply reply = {0};

    int server_coid = name_open(sname, 0);

    if (server_coid == -1) {
        perror("name_open");
        return EXIT_FAILURE;
    }

    printf("Connected to: %s\n", sname);

    while (1)
    {
        // Update sensorData here using keyboard inputs

        // Copy sensor state into outgoing message
        sensorMessage.data = sensorData;

        printf(
            "Sending: NS Vehicle=%d, EW Vehicle=%d, "
            "NS Ped=%d, EW Ped=%d\n",
            sensorData.NS_Vehicle,
            sensorData.EW_Vehicle,
            sensorData.NS_Ped,
            sensorData.EW_Ped
        );

        if (MsgSend(
            server_coid,
            &sensorMessage,
            sizeof(sensorMessage),
            &reply,
            sizeof(reply)
        ) == -1)
        {
            perror("MsgSend");
        }
        else
        {
            printf("Reply: %s\n", reply.buf);
        }

        sleep(5);
    }

    name_close(server_coid);
    return EXIT_SUCCESS;
}

