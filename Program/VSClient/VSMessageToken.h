#pragma once

#include "VSMessageType.h"

#define TokenMaxLength 2048

namespace VS
{
	struct VSMessageToken
	{
		public:
		VSMessageType Type;
		size_t Length;
		char Data[2048];

		VSMessageToken();
		VSMessageToken(VSMessageType type, unsigned int length, char* data);

		void Fill(VSMessageType type, unsigned int length, char* data);

		size_t Parse(char* dataInput, size_t messageLength);
		size_t UnParse(char * dataOutPut);

		static bool CouldBeToken(unsigned char byte);
	};
}