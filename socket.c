#include <winsock2.h>
#include "socket.h"

SOCKET createSocket()
{
    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == INVALID_SOCKET)
        return 0;
    return clientSock;
}

int connectSocket(IRCServer *server, SOCKET sock)
{
    struct sockaddr_in ClientSAddr;
    memset(&ClientSAddr, 0, sizeof(ClientSAddr));

    ClientSAddr.sin_family = AF_INET;
    ClientSAddr.sin_addr.s_addr = inet_addr(server->server);
    ClientSAddr.sin_port = htons(server->port);
    if (connect(sock, (struct sockaddr *) &ClientSAddr, sizeof(ClientSAddr)))
        return 0;
    return 1;
}

void destroySocket(SOCKET sock)
{
    shutdown(sock, 2);
    closesocket(sock);
}