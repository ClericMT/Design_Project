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

// This is where pedestrian/vehicle detection data is stored
typedef struct {
   msg_header_t hdr;
   int ClientID;      		// our data (unique id from client)
   int NS_Vehicle;          // north/south vehicle detected
   int EW_Vehicle;          // east/west vehicle detected
   int NS_Ped;              // north/south ped. crossing request
   int EW_Ped;              // east/west ped. crossing request
} local_sensor_data;

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
	printf("local_sensor_data size= %d\n", sizeof(local_sensor_data));
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
	// Instance of struct to hold data
    local_sensor_data sensorData = { 0x22,600,0,0,0,0 }; // hdr type, ID, NS Vehicle, EW Vehicle, NS Ped., EW Ped.

    my_reply reply; 								// replymsg structure for sending back to client

    int server_coid;

    printf("  ---> Trying to connect to server named: %s\n", sname);
    if ((server_coid = name_open(sname, 0)) == -1)
    {
        printf("\n    ERROR, could not connect to server!\n\n");
        return EXIT_FAILURE;
    }

    printf("Connection established to: %s\n", sname);

    // Console out what is going to be sent (sensor data)
    printf("Client (ID:%d), sending data packet with the integer value: %d \n",
    		sensorData.ClientID, sensorData.NS_Vehicle, sensorData.EW_Vehicle,
			sensorData.NS_Ped, sensorData.EW_Ped);
    fflush(stdout);
    // Actually send the sensor data to LocalController via native message passing
    if (MsgSend(server_coid, &sensorData, sizeof(sensorData), &reply, sizeof(reply)) == -1)
	{
		printf(" Error data NOT sent to server\n"); // maybe we did not get a reply from the server
	}
	else
	{ // now process the reply
		printf("   -->Reply is: '%s'\n", reply.buf);
	}

    sleep(5);

    return EXIT_SUCCESS;
}
