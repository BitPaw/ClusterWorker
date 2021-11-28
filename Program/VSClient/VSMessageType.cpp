#include "VSMessageType.h"

#define MessageInvalidID '?'
#define MessageNameID 'N'
#define MessageGetID 'G'
#define MessagePutID 'P'
#define MessageExecuteID 'E'
#define MessageUploadID 'U'

VS::VSMessageType VS::ConvertMessageType(unsigned char messageType)
{
	switch (messageType)
	{
		case MessageNameID:
			return VS::VSMessageType::Name;

		case MessageGetID: 
			return VS::VSMessageType::Get;

		case MessageExecuteID:
			return VS::VSMessageType::Execute;

		case MessageUploadID:
			return VS::VSMessageType::Upload;

		case MessagePutID: 
			return VS::VSMessageType::Put;

		default: return VS::VSMessageType::Invalid;
			break;
	}
}

unsigned char VS::ConvertMessageType(VSMessageType messageType)
{
	switch (messageType)
	{
		default:
		case VS::VSMessageType::Invalid:
			return MessageInvalidID;

		case VS::VSMessageType::Name:
			return MessageNameID;

		case VS::VSMessageType::Get:
			return MessageGetID;

		case VS::VSMessageType::Put:
			return MessagePutID;

		case VS::VSMessageType::Execute:
			return MessageExecuteID;

		case VS::VSMessageType::Upload:
			return MessageUploadID;
	}
}