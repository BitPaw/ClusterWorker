#include "VSMessageType.h"

#define MessageInvalidID '?'
#define MessageTryingToConnectID 'T'
#define MessageIDLEID 'I'
#define MessageReceiveExecuteableID 'E'
#define MessageReceiveWorkID 'W'
#define MessageSendResultID 'R'
#define MessageSendNameID 'N'	

VS::VSMessageType VS::ConvertMessageType(unsigned char messageType)
{
	switch (messageType)
	{
		case MessageTryingToConnectID:
			return VS::VSMessageType::TryingToConnect;

		case MessageIDLEID:
			return VS::VSMessageType::IDLE;

		case MessageReceiveExecuteableID:
			return VS::VSMessageType::ReceiveExecuteable;

		case MessageReceiveWorkID:
			return VS::VSMessageType::ReceiveWork;

		case MessageSendResultID:
			return VS::VSMessageType::SendResult;

		case MessageSendNameID:
			return VS::VSMessageType::SendName;

		default: 
			return VS::VSMessageType::Invalid;
	}
}

unsigned char VS::ConvertMessageType(VSMessageType messageType)
{
	switch (messageType)
	{
		default:
		case VS::VSMessageType::Invalid:
			return MessageInvalidID;

		case VS::VSMessageType::TryingToConnect:
			return MessageTryingToConnectID;

		case VS::VSMessageType::IDLE:
			return MessageIDLEID;

		case VS::VSMessageType::ReceiveExecuteable:
			return MessageReceiveExecuteableID;

		case VS::VSMessageType::ReceiveWork:
			return MessageReceiveWorkID;

		case VS::VSMessageType::SendResult:
			return MessageSendResultID;

		case VS::VSMessageType::SendName:
			return MessageSendNameID;
	}
}

const char* VS::MessageTypeToString(VSMessageType messageType)
{
	switch (messageType)
	{
		case VS::VSMessageType::Invalid:
			return "Invalid";

		case VS::VSMessageType::TryingToConnect:
			return "TryingToConnect";

		case VS::VSMessageType::IDLE:
			return "IDLE";

		case VS::VSMessageType::ReceiveExecuteable:
			return "ReceiveExecuteable";

		case VS::VSMessageType::ReceiveWork:
			return "ReceiveWork";

		case VS::VSMessageType::SendResult:
			return "SendResult";

		case VS::VSMessageType::SendName:
			return "SendName";

		case VS::VSMessageType::Working:
			return "Working";

		case VS::VSMessageType::ShuttingDown:
			return "ShuttingDown";
	}

	return "Error";
}
