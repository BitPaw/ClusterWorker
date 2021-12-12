#include "ClientInfo.h"

ClientInfo::ClientInfo()
{
	Reset();
}

void ClientInfo::Reset()
{
	Set(-1, ClientState::NotConnected);
}

void ClientInfo::Set(int socketID, ClientState state)
{
	SocketID = socketID;
	State = state;
}