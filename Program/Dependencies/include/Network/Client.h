#pragma once

#include "IOSocket.h"
#include "../Async/Thread.h"

#define IPSize 60

namespace BF
{
    class Client : public IOSocket
    {
        public:
        char IP[IPSize];
        unsigned short Port;

        int ConnectedServerID;
        IOSocket ConnectedServerData;

        Client();

        SocketActionResult ConnectToServer(const char* ip, unsigned short port);
        void Disconnect();

        static ThreadFunctionReturnType CommunicationFunctionAsync(void* ioSocket);
    }; 
}