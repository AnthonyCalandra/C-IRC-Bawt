#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "socket.h"
#include "user.h"
#include "msgstream.h"
#include "curl/curl.h"

int main(int argc, char **argv)
{
    if (argv[1] == NULL)
    {
        printf("Fill in a server IP! (args)");
        return 1;
    }

    if (argv[2] == NULL)
    {
        printf("Fill in a server port! (args)");
        return 1;
    }

    if (argv[3] == NULL)
    {
        printf("Fill in a bot nick! (args)");
        return 1;
    }

    if (argv[4] == NULL)
    {
        printf("Fill in a channel! (args)");
        return 1;
    }

    WSADATA WSStartData;
    WSAStartup(2, &WSStartData);

    SOCKET clientSock = createSocket();
    if (clientSock == 0)
    {
        printf("Problem creating socket!\n");
        destroySocket(clientSock);
        WSACleanup();
        return 1;
    }

    IRCServer *server = (IRCServer *) malloc(sizeof(IRCServer));
    server->server = argv[1];
    server->port = atoi(argv[2]);

    if (!connectSocket(server, clientSock))
    {
        printf("Unable to connect!\n");
        destroySocket(clientSock);
        WSACleanup();
        return 1;
    }

    User *bot1 = (User *) malloc(sizeof(User));
    bot1->channel = argv[4];
    bot1->nick = argv[3];
    bot1->isAlive = 1;
    struct clientSock *aClientSock = (struct clientSock *) malloc(sizeof(struct clientSock));
    aClientSock->clientSock = clientSock;
    bot1->client = aClientSock;

    if (curl_global_init(3) < 0)
    {
        printf("Unable to initialize cURL\n");
        return;
    }

    // Execute some initial commands
    char command[512];
    sprintf(command, "USER %s 8 * :wat\n\rNICK %s\n\r JOIN #%s\n\r", bot1->nick, bot1->nick, bot1->channel);
    sendMessage(bot1->client, command);

    messageLoop(bot1);

    printf("Cleaning up ...");
    free(bot1);
    free(aClientSock);
    destroySocket(clientSock);
    WSACleanup();
    return 0;
}
