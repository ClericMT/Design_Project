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
   msg_header_t hdr;
   int ClientID;      		// our data (unique id from client)
   int NS_Vehicle;          // north/south vehicle detected
   int EW_Vehicle;          // east/west vehicle detected
   int NS_Ped;              // north/south ped. crossing request
   int EW_Ped;              // east/west ped. crossing request
} local_sensor_data;

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


/*** Server code ***/
int server()
{
	printf("_mysigval size= %d\n", sizeof(_mysigval));
	printf("header size= %d\n", sizeof(msg_header_t));
	printf("my_data size= %d\n", sizeof(local_sensor_data));
	printf("my_reply size= %d\n", sizeof(my_reply));

   name_attach_t *attach;

   local_sensor_data sensorData;
   my_reply replymsg; // replymsg structure for sending back to client

   replymsg.hdr.type = 0x01;       // some number to help client process reply msg
   replymsg.hdr.subtype = 0x00;    // some number to help client process reply msg

   // Catch error if can't connect to Local Attach Point
   if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL)
   {
       printf("\nFailed to name_attach on ATTACH_POINT: %s \n", ATTACH_POINT);
       printf("\n Possibly another server with the same name is already running or you need to start the gns service!\n");
	   return EXIT_FAILURE;
   }

   printf("Server Listening for Clients on ATTACH_POINT: %s \n", ATTACH_POINT);

   	/*
	 *  Server Loop
	 */
   int rcvid=0, msgnum=0;  		// no message received yet
   int Stay_alive=1, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)
   living =1;
   while (living)
   {
	   // Do your MsgReceive's here now with the chid
       rcvid = MsgReceive(attach->chid, &sensorData, sizeof(sensorData), NULL);

       if (rcvid == -1)  // Error condition, exit
       {
           printf("\nFailed to MsgReceive\n");
           break;
       }

       if(rcvid > 0) // if true then A message was received
       {
		   msgnum++;

		   // If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
		   if (sensorData.hdr.type == _IO_CONNECT )
		   {
			   MsgReply( rcvid, EOK, NULL, 0 );
			   printf("\nClient messaged indicating that GNS service is running....");
			   printf("\n    -----> replying with: EOK\n");
			   msgnum--;
			   continue;	// go back to top of while loop
		   }

		   // Some other I/O message was received; reject it
		   if (sensorData.hdr.type > _IO_BASE && sensorData.hdr.type <= _IO_MAX )
		   {
			   MsgError( rcvid, ENOSYS );
			   printf("\n Server received and IO message and rejected it....");
			   continue;	// go back to top of while loop
		   }

		   // A message (presumably ours) received

		   // put your message handling code here and assemble a reply message
		   sprintf(replymsg.buf, "Message %d received", msgnum);
		   printf("\nServer received data from from local sensor (ID:%d):\n\nNS Vehcile Detected: \t\t%d \nEW Vehicle Detected: \t\t%d \nNS Pedestrian Detected: \t%d \nEW Pedestrian Detected: \t%d",
				   sensorData.ClientID, sensorData.NS_Vehicle, sensorData.EW_Vehicle,
					sensorData.NS_Ped, sensorData.EW_Ped);
		   	   fflush(stdout);
		   	  // sleep(1); // Delay the reply by a second (just for demonstration purposes)

		   printf("\n\n    -----> replying with: '%s'\n",replymsg.buf);
		   MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
       }
       else
       {
		   printf("\nERROR: Server received something, but could not handle it correctly\n");
       }

   }

   // Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
   name_detach(attach, 0);

   return EXIT_SUCCESS;
}
