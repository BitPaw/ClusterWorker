#pragma once

#include "ClientInfo.h"

struct ClientList
{
    private:
    size_t _clientInfoListSizeCurrent;
    size_t _clientInfoListSizeMaximal;
    ClientInfo* _clientInfoList;

	public:
    ClientList();
    ~ClientList();

    ClientInfo* Register(int socketID);
    void UnRegister(int socketID);
    ClientInfo* GetNextWaiting();
    ClientInfo* GetViaID(int socketID);
};