#include "VSMessageType.h"

#define MessageInvalidID '?'
#define MessageNameID 'N'
#define MessageAnswerID 'A'
#define MessageQuestionID 'Q'
#define MessageExecuteID 'E'
#define MessageUploadID 'U'
#define MessagePerformanceHardwareID 'P'
#define MessageFinishedID 'F'

VS::VSMessageType VS::ConvertMessageType(unsigned char messageType)
{
	switch (messageType)
	{
		case MessageFinishedID:
			return VS::VSMessageType::Finished;

		case MessageNameID:
			return VS::VSMessageType::Name;

		case MessageAnswerID: 
			return VS::VSMessageType::Answer;

		case MessageExecuteID:
			return VS::VSMessageType::Execute;

		case MessageUploadID:
			return VS::VSMessageType::Upload;

		case MessageQuestionID: 
			return VS::VSMessageType::Question;

		case MessagePerformanceHardwareID:
			return VS::VSMessageType::PerformanceHardware;

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

		case VS::VSMessageType::Finished:
			return MessageFinishedID;

		case VS::VSMessageType::Name:
			return MessageNameID;

		case VS::VSMessageType::Answer:
			return MessageAnswerID;

		case VS::VSMessageType::Question:
			return MessageQuestionID;

		case VS::VSMessageType::Execute:
			return MessageExecuteID;

		case VS::VSMessageType::Upload:
			return MessageUploadID;

		case VS::VSMessageType::PerformanceHardware:
			return MessagePerformanceHardwareID;
	}
}