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

size_t VS::VSMessageToken::Parse(char* dataInput)
{
	Type = ConvertMessageType(dataInput[0]);
	Length = 
		(unsigned int)dataInput[1] << 24 |
		(unsigned int)dataInput[2] << 16 |
		(unsigned int)dataInput[3] << 8 |
		(unsigned int)dataInput[4];

	memcpy(Data, dataInput + 5, Length);

	return 5u + Length;
}

size_t VS::VSMessageToken::UnParse(char* dataOutPut)
{
	dataOutPut[0] = ConvertMessageType(Type);
	dataOutPut[1] = (Length & 0xFF000000) >> 24;
	dataOutPut[2] = (Length & 0x00FF0000) >> 16;
	dataOutPut[3] = (Length & 0x0000FF00) >> 8;
	dataOutPut[4] = Length & 0x000000FF;

	memcpy(dataOutPut + 5, Data, Length);

	return 5u + Length;
}

bool VS::VSMessageToken::CouldBeToken(unsigned char byte)
{
	return 'A' <= byte && byte <= 'Z';
}