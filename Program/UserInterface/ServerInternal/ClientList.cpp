#include "ClientList.h"

#include <stdlib.h>

ClientList::ClientList()
{
    _clientInfoListSizeCurrent = 0;
    _clientInfoListSizeMaximal = 10;
    _clientInfoList = new ClientInfo[_clientInfoListSizeMaximal];
}

ClientList::~ClientList()
{
    delete[] _clientInfoList;
}

ClientInfo* ClientList::Register(int socketID)
{
    bool hasEnoghMemory = _clientInfoListSizeCurrent + 1 <= _clientInfoListSizeMaximal;

    if (hasEnoghMemory)
    {
        ClientInfo* clientInfo = GetViaID(-1);

        if (!clientInfo)
        {
            return nullptr; // Error, should not happen
        }

        clientInfo->Set(socketID, ClientState::IDLE);

        ++_clientInfoListSizeCurrent;

        return clientInfo;
    }
    else
    {
        _clientInfoListSizeMaximal += 10;

        ClientInfo* newArray = (ClientInfo*)realloc(_clientInfoList, sizeof(ClientInfo) * _clientInfoListSizeMaximal);

        if (!newArray)
        {
            return nullptr;// outOfMemory;
        }

        return Register(socketID);
    }
}

void ClientList::UnRegister(int socketID)
{
    ClientInfo* clientInfo = GetViaID(socketID);

    if (!clientInfo)
    {
        return; // Error, not found
    }

    clientInfo->Reset();

    --_clientInfoListSizeCurrent;
}

ClientInfo* ClientList::GetNextWaiting()
{
    for (size_t i = 0; i < _clientInfoListSizeMaximal; i++)
    {
        ClientInfo& clientInfo = _clientInfoList[i];

        if (clientInfo.State == ClientState::IDLE)
        {
            return &_clientInfoList[i];
        }
    }

    return nullptr;
}

ClientInfo* ClientList::GetViaID(int socketID)
{
    for (size_t i = 0; i < _clientInfoListSizeMaximal; i++)
    {
        ClientInfo& clientInfo = _clientInfoList[i];

        if (clientInfo.SocketID == socketID)
        {
            return &_clientInfoList[i];
        }
    }

    return nullptr;
}