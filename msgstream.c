#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "user.h"
#include "msgstream.h"

void messageLoop(User *bot)
{
    char buffer[512];
    int received;
    while (received = recv(bot->client->clientSock, buffer, 511, 0) != 0)
    {
        puts(buffer);
        parseMessage(bot, buffer);
        memset(&buffer, 0, 512);
        if (bot->isAlive == 0)
            break;
    }
}

int sendMessage(struct clientSock *sock, char command[512])
{
    if (send(sock->clientSock, command, strlen(command), 0) != 0)
        return 1;
    return 0;
}

