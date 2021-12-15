#include "ClientState.h"

const char* ClientStateToString(ClientState clientState)
{
	switch (clientState)
	{
		case ClientState::NotConnected:
			return "Not Connected";

		case ClientState::IDLE:
			return "IDLE";

		case ClientState::Working:
			return "Working";

		default:
			return "Error";
	}
}
