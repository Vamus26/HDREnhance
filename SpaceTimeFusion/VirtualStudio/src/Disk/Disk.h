#ifndef __DISK_H__
#define __DISK_H__

#include "Definitions.h"

class Disk
{

public:

	static int NumOfMatchesInPattern(string pattern);
	static vector<string> MatchesInPattern(string pattern);
	static void MatchesInPattern(string pattern, vector<string> &fileNames);
	
	static bool DoesDirExist(string fileName);
	static void MakeDir(string dirName);

	static bool DoesFileExist(const char* fileName);
	static bool DoesFileExist(string fileName);
	static void RenameFile(string orgFN, string targetFN, bool overwriteIfTargetExists = false);
	static void CopyFile(string orgFN, string targetFN);
	static void DeleteFile(string fileName);
	static string GetBaseFileName(string fullFileName);
	static string GetBaseDirName(string fullFileName);
	static string GetFileNameID(string fullFileName);
	static string GetBasePathName(string fullFileName);

	static char* LoadBinFile(string fileName, int &memSize);
	static void SaveBinFile(char *memBuff, int memSize, string fileName);
};

#endif

