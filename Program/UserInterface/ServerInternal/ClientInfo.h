#pragma once

#include "ClientState.h"
#include <cstdio>

struct ClientInfo
{
	public:
	int SocketID;
	ClientState State;

	char Name[128];

	char CurrentFileName[260];
	FILE* CurrentFile;

	ClientInfo();

	void Reset();
	void Set(int socketID, ClientState state);
};