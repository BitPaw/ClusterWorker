#include <cstdio>
#include <iostream>
#include <Time/StopWatch.h>
#include <File/FileStream.h>
#include <stdlib.h>
#include <Windows.h>
#include <string>

using namespace BF;


bool isPrime(int n) {
	int i, m = 0, flag = 0;
	m = n / 2;
	for (i = 2; i <= m; i++)
	{
		if (n % i == 0)
		{
			//std::cout << "Number is not Prime." << std::endl;
			flag = 1;
			break;
		}
	}
	if (flag == 0)
	{
		//std::cout << "Number is Prime." << std::endl;
		return 1;
	}
	return 0;
}

int main(int numberOfParameters, char** parameterList)
{
	if (numberOfParameters != 2)
		return -3;
	
	char* path = parameterList[1];
	const char* outPath = "out.result";
	//printf("%s\n", path);
	FileStream fs;
	

	fs.ReadFromDisk(path);//"C:/Users/Merdo/Desktop/-- VS/ClusterWorker/Program/x64/Debug/in.chunk");

	char s[2048];

	int begin = 0;
	int end = 0;
	
	if(fs.ReadNextLineInto(s))
		begin = std::stoi(s);
		//printf("%i\n", begin);
	
	if (fs.ReadNextLineInto(s))
		end = std::stoi(s);
		//printf("%i\n", end);
	
	StopWatch stopWatch;
	stopWatch.Start();


	std::string out;
	
	for (int i = begin; i < end; ++i)
	{
		if(isPrime(i))
		{
			//printf("%i\n", i);
			out += std::to_string(i) + "\n";
		}
	}

	const float x = stopWatch.Stop();
		
	printf("Calculated finished in: %fs\n", x);
	
	FILE* stream;
	
	fopen_s(&stream, outPath, "wb+");
	if (stream) {
		fprintf_s(stream, out.c_str());
		fclose(stream);
	}
	
	return EXIT_SUCCESS;
}


