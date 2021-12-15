#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include <cstdio>
#include <cstdlib>

int main(int numberOfParameters, char** parameterList)
{
	char buffer[1024];
	FILE* file = fopen("A:/_Cache/TEST.txt", "rb+");

	if (!file)
	{
		EACCES;
		printf("Error %d \n", errno);
		return EXIT_FAILURE;
	}

	size_t writtenBytes = sprintf(buffer, "Number of Paramaters : %i\n", numberOfParameters);

	fwrite(buffer, sizeof(char), writtenBytes, file);

	for (size_t i = 0; i < numberOfParameters; i++)
	{
		writtenBytes = sprintf(buffer, "Paramater (%i/%i): %s\n", i, numberOfParameters, parameterList[i]);

		fwrite(buffer, sizeof(char), writtenBytes, file);
	}

	fclose(file);

	Sleep(2000);

	return EXIT_SUCCESS;
}