#pragma once

namespace VS
{
	enum class VSMessageType
	{
		Invalid, // Should not be

		TryingToConnect,
		
		IDLE, // Connecting and Waiting for action

		ReceiveExecuteable,
		ReceiveWork,

		SendResult,
		SendName,

		Working,

		ShuttingDown
	};

	VSMessageType ConvertMessageType(unsigned char messageType);
	unsigned char  ConvertMessageType(VSMessageType messageType);

	const char* MessageTypeToString(VSMessageType messageType);
}