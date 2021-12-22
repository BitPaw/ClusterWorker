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
	printf("%s", path);
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

void Combine(const char* filePathInput, const char* filePathOutput)
{
	FileStream fs;
	fs.ReadFromDisk(filePathInput);
	char s[2048];	
	FILE* stream = nullptr;
	fopen_s(&stream, filePathOutput, "wb+");
	
	while(fs.ReadNextLineInto(s))
	{
		//printf("%s\n",s);
		char chunkPath[260];
		char innerS[2048];
		
		FileStream fs2;
		fs2.ReadFromDisk(chunkPath);
		sprintf_s(chunkPath, "%s/%s", _workingDirectory, s);
		//printf("%s\n", chunkPath);
		while (fs2.ReadNextLineInto(innerS))
		{
			if (stream)
			{
				fprintf_s(stream,"%s\n", innerS);
			}
		}
	}
	if (stream)
		fclose(stream);
}

//---<Index>--------------------------
#define IndexMode 1
#define IndexInputFile 2
#define IndexOutPutFile 3
#define IndexWorkDirectory 4
//---<Modes>--------------------------
#define ModeSplitt 0
#define ModeCombine 1
//---<Exit Codes>---------------------
#define ExitInvalidParameters -10
#define ExitInvalidMode -11
//------------------------------------

int main(int numberOfParameters, char** parameterList)
{
	if (numberOfParameters != 5)
	{
		return ExitInvalidParameters;
	}

	const char* mode = parameterList[IndexMode];
	const char* inputFilePath = parameterList[IndexInputFile];
	const char* outputFilePath = parameterList[IndexOutPutFile];
	_workingDirectory = parameterList[IndexWorkDirectory];
	const int modeID = mode[0] - '0';

	switch (modeID)
	{
		case ModeSplitt:
		{
			Splitter(inputFilePath);
			return EXIT_SUCCESS;
		}
		case ModeCombine:
		{
			Combine(inputFilePath, outputFilePath);
			break;			
		}

		default:
		{
			return ExitInvalidMode;
		}		
	}

	return EXIT_SUCCESS;
}