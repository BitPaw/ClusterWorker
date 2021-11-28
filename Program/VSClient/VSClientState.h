#pragma once

namespace VS
{
	enum class VSClientState
	{
		Invalid,

		Startup, // Initialisation
	
		IDLE, // Client searches for connection 
		Communicating, // Client send or recieves data
		Calculating, // Client does work

		Execute, // Client Shall start the executable

		Updating, // Client updates application

		Stop // Client is now stopping
	};
}