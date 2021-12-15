#pragma once

enum class ClientState
{
	NotConnected,

	IDLE, // Client is free and can take on some work.
	Working, // Client is calculating somethin and will respons soon.

	SendingResult
};

const char* ClientStateToString(ClientState clientState);