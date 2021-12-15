#include <cstdio>
#include <Time/StopWatch.h>
#include <stdlib.h>
#include <Windows.h>
#include <File/FileStream.h>
#include <string>

using namespace BF;

const char* _workingDirectory = nullptr;

void FileOut(int chunkNr, std::string out)
{
	char path[260];
	FILE* stream = nullptr;

	sprintf_s(path, "%s/%i.chunk", _workingDirectory, chunkNr);

	fopen_s(&stream, path, "wb+");

	if (stream) 
	{
		fprintf_s(stream, out.c_str());
		fclose(stream);
	}
}

void Splitter(const char* path)
{
	FileStream fs;
	
	fs.ReadFromDisk(path);
	//fs.ReadFromDisk("C:/Users/Merdo/Desktop/-- VS/ClusterWorker/Program/x64/Debug/in.input");

	char s[2048];

	int begin = 0;
	int end = 0;
	int split = 0;

	if (fs.ReadNextLineInto(s))
		begin = std::stoi(s);
	//printf("%i\n", begin);

	if (fs.ReadNextLineInto(s))
		end = std::stoi(s);
	//printf("%i\n", end);

	if (fs.ReadNextLineInto(s))
		split = std::stoi(s);
	//printf("%i\n", split);

	const int totalCount = end - begin;
	const int splitAt = totalCount / split;
	const int remaning = totalCount % split;

	for (int i = 0; i < split; ++i)
	{
		const int chunkStart = begin + splitAt * i;
		const int chunkEnd = begin + splitAt * (i + 1) + (i != split - 1 ? -1 : 0);
		//printf("%i - ", chunkStart);
		//printf("%i\n", chunkEnd);

		const std::string out = std::to_string(chunkStart) + "\n" + std::to_string(chunkEnd);

		FileOut(i, out);
	}

	if (remaning != 0)
	{
		//printf("%i - ", end - remaning + 1);
		//printf("%i\n", end);

		const std::string out = std::to_string(end - remaning + 1) + "\n" + std::to_string(end);

		FileOut(split, out);
	}
}

void Combine(const char* filePathOutput)
{
	//TODO: getAllResults and Combine
}

#define ModeSplitt 0
#define ModeCombine 1
#define ExitInvalidParameters -10
#define ExitInvalidMode -11

int main(int numberOfParameters, char** parameterList)
{
	if (numberOfParameters != 5)
	{
		return ExitInvalidParameters;
	}

	const char* mode = parameterList[1];
	const char* inputFilePath = parameterList[2];
	const char* outputFilePath = parameterList[3];
	_workingDirectory = parameterList[4];
	const int modeID = mode[0] - '0';

	switch (modeID)
	{
		case ModeSplitt:
		{
			Splitter(inputFilePath);
			break;
		}
		case ModeCombine:
		{
			Combine(outputFilePath);
			break;			
		}

		default:
		{
			return ExitInvalidMode;
		}		
	}

	return EXIT_SUCCESS;
}