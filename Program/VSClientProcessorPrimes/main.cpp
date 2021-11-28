#include <cstdio>
#include <Time/StopWatch.h>
#include <stdlib.h>
#include <Windows.h>

using namespace BF;

int main(int numberOfParameters, char** parameterList)
{
	StopWatch stopWatch;

	stopWatch.Start();

	Sleep(100);

	float x = stopWatch.Stop();

	printf("Hello %f\n", x);

	return EXIT_SUCCESS;
}