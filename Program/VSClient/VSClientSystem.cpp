#include "VSClientSystem.h"
#include <Network/SocketActionResult.h>
#include <Container/AsciiString.h>

using namespace BF;

void VSClientSystem::Initialisize()
{
	_client.Callback = this;
}

void VSClientSystem::StartConnectingLoop(const char* ipString, const char* portString)
{
    SocketActionResult socketActionResult = SocketActionResult::InvalidResult;
    bool isConnected = false;
    unsigned short port = 25565;

    printf("[i] Starting to connect...\n");

    while (!isConnected)
    {
        SocketActionResult socketActionResult = _client.ConnectToServer(ipString, port);

        isConnected = socketActionResult == SocketActionResult::Successful;

        if (!isConnected)
        {
            const unsigned short sleepInMilliseconds = 3000;

            printf("[i] Connection failed! Waiting <%i> seconds to try again...\n", sleepInMilliseconds / 1000);

            Sleep(sleepInMilliseconds);
        }
    }
}

void VSClientSystem::OnConnectionLinked(int socketID)
{
    printf("[i][%i] Connection linked.\n", socketID);
}

void VSClientSystem::OnConnectionListening(int socketID)
{
    printf("[i][%i] Connection listening.\n", socketID);
}

void VSClientSystem::OnConnectionEstablished(int socketID)
{
    printf("[i][%i] Connection established.\n", socketID);
}

void VSClientSystem::OnConnectionTerminated(int socketID)
{
    printf("[i][%i] Connection terminated.\n", socketID);
}

void VSClientSystem::OnMessageSend(int socketID, const char* message, size_t messageSize)
{
    printf("[i][%i-You] Message:%s (%i Bytes).\n", socketID, message, messageSize);
}

void VSClientSystem::OnMessageReceive(int socketID, const char* message, size_t messageSize)
{
    printf("[i][%i-Server] Message:%s (%i Bytes).\n", socketID, message, messageSize);
}