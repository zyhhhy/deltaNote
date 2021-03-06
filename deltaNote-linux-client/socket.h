#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "pack.h"
#include "log.h"

using namespace std;

#define SOCKET_ERROR (-1)
#define SOCKET_SUCCESS (0)

enum SocketState {
    SocketRunning = 1,
    SocketStopped = 2,
    SocketConnSuccess = 3,
    SocketKilled = 4,
    SocketError=0
};

class SocketClient{
public:
    SocketClient();
    SocketClient(char *_serverIP, int _serverPort);
    ~SocketClient();

    int sendMsg(void* buf,size_t size);
    int recvMsg(void* buf,size_t size);
    SocketState closeClient();
    SocketState getSocketOpState();

private:
    char serverIP[G_ARR_SIZE_SERVER]{};
    int serverPort;

    int ret;
    SocketState socketState;

    int clientSocketFd;
};
#endif // SOCKET_H
