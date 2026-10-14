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

#define MSG_CENTRAL_COMMAND 0x23
#define MSG_CONTROLLER 0x24

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

// This is where pedestrian/vehicle detection data is stored
typedef struct {
   int NS_Vehicle;          // north/south vehicle detected
   int EW_Vehicle;          // east/west vehicle detected
   int NS_Ped;              // north/south ped. crossing request
   int EW_Ped;              // east/west ped. crossing request
} SensorData;

// Data received from CentralController
typedef struct {
    int mode;
    int boomGateDown;
    int boomGateConnected;
} CentralCommand;

typedef struct {
    msg_header_t hdr;
    int ClientID;
    CentralCommand data;
} CentralCommandMessage;

// traffic signal state
typedef enum {
    R = 'R', Y = 'Y', G = 'G'
} TrafficSignal;

// Traffic Signals for local intersection
typedef struct {
	TrafficSignal NSSLight;
	TrafficSignal NSRLight;
	TrafficSignal EWSLight;
	TrafficSignal EWRLight;
	TrafficSignal NSPedLight;
	TrafficSignal EWPedLight;
} TrafficSignals;

//
typedef struct {
    int ID;                     // Intersection ID (i.e I1, I2)
    int active;					// Intersection online / offline status
    CentralCommand centralCommand;
    TrafficSignals trafficSignals;
    SensorData sensorData;
} IntersectionStatus;

//
typedef struct {
	msg_header_t hdr;
    int ID;                     // Intersection ID (i.e I1, I2)
    int active;					// Intersection online / offline status
    IntersectionStatus intersectionStatus;
} IntersectionStatusMessage;

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
    CentralCommand centralCommand = {0};
    CentralCommandMessage centralCommandMessage = {0};

    centralCommandMessage.hdr.type = MSG_CENTRAL_COMMAND;
    centralCommandMessage.hdr.subtype = 0x00;
    centralCommandMessage.ClientID = 601;

    centralCommand.mode = 1; // sensor driven mode

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
        centralCommandMessage.data = centralCommand;

        printf(
            "Sending: Operation mode=%d, Boom gate Down=%d, "
            "Boom gate Connected=%d\n",
            centralCommand.mode, centralCommand.boomGateDown, centralCommand.boomGateConnected
        );

        if (MsgSend(
            server_coid,
            &centralCommandMessage,
            sizeof(centralCommandMessage),
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

