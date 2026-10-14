// Client 2020
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "app.h"

#define QNET_ATTACH_POINT \
	"/net/VM_x86_Target01/dev/name/local/LocalController" // use as attach point for passing mesages between processes in different nodes

#define MSG_CENTRAL_COMMAND 0x23
#define MSG_INTERSECTION_STATUS 0x24

// prototype
int client(char *sname);

int main(int argc, char *argv[])
{
    int ret = 0;
    ret = client(QNET_ATTACH_POINT);  // Init. client code

	printf("CentralController Terminated....\n");
	return ret;
}

// Client code
int client(char *sname)
{
    CentralCommand centralCommand = {0};
    CentralCommandMessage centralCommandMessage = {0};

    centralCommandMessage.hdr.type = MSG_CENTRAL_COMMAND;
    centralCommandMessage.ClientID = 601;

    centralCommand.mode = 1; // sensor driven mode

    my_reply reply = {0};

    int server_coid = name_open(sname, 0);

    // Error handling
    if (server_coid == -1) {
        perror("name_open");
        return EXIT_FAILURE;
    }

    printf("Connected to: %s\n", sname);

    while (1)
    {
        // Copy sensor state into outgoing message
        centralCommandMessage.data = centralCommand;

        printf(
            "Sending: Operation mode=%d, Boom gate Down=%d, "
            "Boom gate Connected=%d\n",
            centralCommand.mode, centralCommand.boomGateDown, centralCommand.boomGateConnected
        );

        // Send message
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

