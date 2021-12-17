#pragma once

struct UserSafeFile
{
	public:
	char FilePathInput[260];
	char FilePathServer[260];
	char FilePathClient[260];
	char FilePathOutPut[260];

	UserSafeFile();

	void Load();
	void Save();

	void Reset();
};