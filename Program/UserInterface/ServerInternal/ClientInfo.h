#pragma once

#include "ClientState.h"

struct ClientInfo
{
	public:
	int SocketID;
	ClientState State;

	ClientInfo();

	void Reset();
	void Set(int socketID, ClientState state);
};