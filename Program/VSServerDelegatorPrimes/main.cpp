#include <cstdio>
#include <Time/StopWatch.h>
#include <stdlib.h>
#include <Windows.h>
#include <File/FileStream.h>

using namespace BF;

void FileOut(int chunkNr, std::string out)
{
	std::string path;// = "C:/Users/Merdo/Desktop/-- VS/ClusterWorker/Program/x64/Debug/";
	path += std::to_string(chunkNr);
	path += ".chunk";

	FILE* stream;
	fopen_s(&stream, path.c_str(), "w+");
	if (stream) {
		fprintf_s(stream, out.c_str());
		fclose(stream);
	}
}

void Splitter(char* path)
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

void Combine()
{
	//TODO: getAllResults and Combine
}

int main(int numberOfParameters, char** parameterList)
{
	const int mode = *parameterList[1] - '0';
	//printf("%i\n", mode);
	
	if (numberOfParameters == 2 && mode == 1)
	{
		Combine();
		return EXIT_SUCCESS;
	}
	
	if (numberOfParameters == 3 && mode == 0)
	{
		char* path = parameterList[2];
		//printf("%s\n", path);

		Splitter(path);
		return EXIT_SUCCESS;
	}

	return -1;
}