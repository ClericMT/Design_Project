// Server 2020
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "app.h"

// Local NativeMsgPass between server (LocalController.c) and client (LocalSensor.c)
// Derived from Lab5 NativeMsgPass-Client.c

// Local attach point (on same node) for client/server located at <hostname>/dev/name/local/LocalController"
#define ATTACH_POINT "LocalController"  // This must be the same name that is used for the client.

#define MSG_SENSOR_DATA 	0x22
#define MSG_CENTRAL_COMMAND 0x23

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



// Process received sensor data
void handleSensorData(IntersectionStatus* status, const SensorData* data)
{
	status->sensorData = *data;

    printf("\nReceived sensor data:\n");
    printf("  NS Vehicle: %d\n", data->NS_Vehicle);
    printf("  EW Vehicle: %d\n", data->EW_Vehicle);
    printf("  NS Pedestrian: %d\n", data->NS_Ped);
    printf("  EW Pedestrian: %d\n", data->EW_Ped);
}

// Process received central commands
void handleCentralCommand(IntersectionStatus* status, const CentralCommand* command)
{
	status->centralCommand = *command;

	printf("\nReceived CentralController command:\n");
	printf("  Mode: %d\n", command->mode);
	printf("  Boom gate down: %d\n", command->boomGateDown);
	printf("  Boom gate connected: %d\n", command->boomGateConnected);
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

	IntersectionStatus status = {0};

	status.ID = 1;
	status.active = 1;

    name_attach_t* attach;

    IncomingMessage msg = {0};
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
    	    &msg,
    	    sizeof(msg),
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
            handlePulse((const struct _pulse*)&msg);
            continue;
        }

        // QNX connection request
        if (msg.hdr.type == _IO_CONNECT)
        {
            MsgReply(rcvid, EOK, NULL, 0);
            continue;
        }

        // Unsupported QNX I/O message
        if (msg.hdr.type > _IO_BASE &&
            msg.hdr.type <= _IO_MAX)
        {
            MsgError(rcvid, ENOSYS);
            continue;
        }

        // Validate application message type
        if (msg.hdr.type != MSG_SENSOR_DATA && msg.hdr.type != MSG_CENTRAL_COMMAND)
        {
            MsgError(rcvid, ENOSYS);
            continue;
        }


        switch (msg.hdr.type) {
        case MSG_SENSOR_DATA:
				// Process sensor data
		        handleSensorData(&status, &msg.sensor.data);
        		break;
        case MSG_CENTRAL_COMMAND:
        	// Process central commands
			handleCentralCommand(&status, &msg.sensor.data);
			break;
        }

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

