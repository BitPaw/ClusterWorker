#include "VSMessageToken.h"

#include <string.h>

VS::VSMessageToken::VSMessageToken()
{
	Fill(VSMessageType::Invalid, 0, nullptr);
}

VS::VSMessageToken::VSMessageToken(VSMessageType type, unsigned int length, char* data)
{
	Fill(type, length, data);
}

void VS::VSMessageToken::Fill(VSMessageType type, unsigned int length, char* data)
{
	Type = type;
	Length = length;

	memset(Data, 0, TokenMaxLength);
	memcpy_s(Data, TokenMaxLength, data, length);
}

size_t VS::VSMessageToken::Parse(char* dataInput, size_t messageLength)
{
	unsigned char type = dataInput[0];

	Type = ConvertMessageType(type);
	Length = messageLength;

	memcpy(Data, dataInput + 1, messageLength -1);

	return messageLength;
}

size_t VS::VSMessageToken::UnParse(char* dataOutPut)
{
	dataOutPut[0] = ConvertMessageType(Type);

	memcpy(dataOutPut + 1, Data, Length);

	return 1u + Length;
}

bool VS::VSMessageToken::CouldBeToken(unsigned char byte)
{
	return 'A' <= byte && byte <= 'Z';
}