#include "UserSafeFile.h"

#include <string>

#include <File/File.h>
#include <File/FileStream.h>

#define FilePathSafeFile "UISaveFile.sav"

UserSafeFile::UserSafeFile()
{
	Reset();
}

void UserSafeFile::Load()
{
	bool doesFileExist = BF::File::DoesFileExist(FilePathSafeFile);	

	if (doesFileExist)
	{
		BF::FileStream file;

		file.ReadFromDisk(FilePathSafeFile, true);

		file.ReadNextLineInto(FilePathInput);
		file.ReadNextLineInto(FilePathServer);
		file.ReadNextLineInto(FilePathClient);
		file.ReadNextLineInto(FilePathOutPut);
	}
}

void UserSafeFile::Save()
{
	BF::File::Remove(FilePathSafeFile);
	BF::FileStream fileStream;

	unsigned char fileBuffer[2048];

	size_t bytesToWrite = sprintf((char*)fileBuffer, "%s\n%s\n%s\n%s", FilePathInput, FilePathServer, FilePathClient, FilePathOutPut);

	fileStream.Data = fileBuffer;
	fileStream.DataSize = bytesToWrite;

	fileStream.WriteToDisk(FilePathSafeFile);

	fileStream.Data = 0;
	fileStream.DataSize = 0;
}

void UserSafeFile::Reset()
{
	memset(FilePathInput, 0, sizeof(FilePathInput));
	memset(FilePathServer, 0, sizeof(FilePathServer));
	memset(FilePathClient, 0, sizeof(FilePathClient));
	memset(FilePathOutPut, 0, sizeof(FilePathOutPut));
}