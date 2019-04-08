#include "Disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include<algorithm>
#pragma warning(disable:4996)

int Disk::NumOfMatchesInPattern(string pattern)
{
   vector<string> fileNames;
   Disk::MatchesInPattern(pattern, fileNames);
   return (int) fileNames.size();
}

vector<string> Disk::MatchesInPattern(string pattern)
{
	vector<string> fileNames;
	Disk::MatchesInPattern(pattern, fileNames);
	return fileNames;
}

void Disk::MakeDir(string dirName)
{
	if(DoesDirExist(dirName) == false)
	{
		if(MKDIR(dirName.c_str(), 0733) != 0)
		{
			REPORT_FAILURE("Could not create dir - %s\n", dirName.c_str());
		}
	}
}


bool Disk::DoesFileExist(const char* fileName)
{	
	FILE* fp = fopen(fileName, "r");
	if(fp != NULL)
	{
		fclose(fp);
		return true;
	}
	else
	{
		return false;
	}
}

bool Disk::DoesFileExist(string fileName)
{
	return DoesFileExist(fileName.c_str());
}

void Disk::DeleteFile(string fileName)
{
	if(DoesFileExist(fileName))
	{
		if(remove(fileName.c_str()) != 0)
		{
			 REPORT_FAILURE("Could not delete file - %s\n", fileName.c_str());
		}
	}
}

void Disk::RenameFile(string orgFN, string targetFN, bool overwriteIfTargetExists)
{
	if(orgFN.compare(targetFN) != 0)
	{
		if(overwriteIfTargetExists == true)
		{
			if(Disk::DoesFileExist(targetFN))
			{
				Disk::DeleteFile(targetFN);
			}
		}
		if(rename(orgFN.c_str(), targetFN.c_str()) != 0)
		{
			 REPORT_FAILURE("Could not rename %s to %s\n",
							orgFN.c_str(), targetFN.c_str());
		}
	}
}


void Disk::CopyFile(string orgFN, string targetFN)
{
	char cmdBuf[2048];
	sprintf(cmdBuf, "%s %s %s\n", COPY_CMD, orgFN.c_str(), targetFN.c_str());
	int returnVal = system(cmdBuf);
	INSIST(returnVal == 0);
}

string Disk::GetBasePathName(string fullFileName)
{
	size_t slashIndex = fullFileName.find_last_of(OS_PATH_DELIMIT, fullFileName.size() - 2);
	if(slashIndex == string::npos)
		return "." + OS_PATH_DELIMIT_STR;
	else
		return fullFileName.substr(0, slashIndex + 1);
}

string Disk::GetBaseFileName(string fullFileName)
{
	vector<string> tokens;
	Tokenize(fullFileName, tokens, "\\/");
	ENSURE(tokens.size() != 0);
	return tokens[tokens.size() - 1];
}

string Disk::GetBaseDirName(string fullFileName)
{
	return GetBaseFileName(fullFileName);
}

string Disk::GetFileNameID(string fullFileName)
{
	size_t dotIndex = fullFileName.find_last_of('.');
	if(dotIndex == string::npos)
		return fullFileName;
	else
		return fullFileName.substr(0, dotIndex);
}


char* Disk::LoadBinFile(string fileName, int &memSize)
{
	FILE *inFile = fopen(fileName.c_str(), "rb");
	ENSURE(inFile != NULL);

	int seekResult;
	seekResult = fseek(inFile, 0, SEEK_END);
	ENSURE(seekResult != -1);

	memSize = (int) ftell(inFile);
	ENSURE(memSize > 0);

	seekResult = fseek(inFile, 0, SEEK_SET);
	ENSURE(seekResult != -1);

	char *memBuff = new char[memSize];
	ENSURE(memBuff != NULL);

	int byteToRead = memSize;
	int totalBytesRead = 0;
	while(byteToRead != 0)
	{
		int freadCount;
		#ifdef WIN32
			freadCount = min(byteToRead, 64000);
		#else
			freadCount = byteToRead;
		#endif

		int bytesRead = (int) fread((void *) (memBuff + totalBytesRead), 
										sizeof(*memBuff),
										freadCount,
										inFile);

		totalBytesRead += bytesRead;
		byteToRead -= bytesRead;

		if(bytesRead == 0)
		{
			int errorCode = ferror(inFile);
			if(errorCode != 0)
			{
				PRINT_INT(errorCode);
				PRINT_INT(errno);
				PRINT_STR(strerror(errno));
				REPORT_FAILURE("IO failure");
			}
		}	
	}

	fclose(inFile);

	return memBuff;
}

void Disk::SaveBinFile(char *memBuff, int memSize, string fileName)
{
	ENSURE(memBuff != NULL);

	FILE *outFile = fopen(fileName.c_str(), "wb");
	ENSURE(outFile != NULL);

	int byteToWrite = memSize;
	int totalBytesWritten = 0;
	while(byteToWrite != 0)
	{
		int bytesWritten = (int) fwrite(memBuff + totalBytesWritten, 
										sizeof(*memBuff), 
										byteToWrite,  
										outFile);

		totalBytesWritten += bytesWritten;
		byteToWrite -= bytesWritten;
	}

	fclose(outFile);
}

#ifdef WIN32
	void Disk::MatchesInPattern(string pattern, vector<string> &fileNames)
	{
	   ENSURE(fileNames.size() == 0);

	   string path = "";
	   size_t pathLength = pattern.find_last_of(OS_PATH_DELIMIT);
	   if(pathLength != -1)
	   {
		   path = pattern.substr(0, pathLength + 1);
	   }


	   struct _finddata_t fileInfo;
	   intptr_t patternHandle;   
	   patternHandle = _findfirst(pattern.c_str(), &fileInfo);
	   if(patternHandle != -1)
	   {
		   do
		   {
			   string newFile = path + fileInfo.name;		   
			   fileNames.push_back(newFile);
		   } while(_findnext(patternHandle, &fileInfo) != -1);
		   _findclose(patternHandle);	   
	   }
	}

	bool Disk::DoesDirExist(string dirName)
	{
	   if(dirName[dirName.size() - 1] != OS_PATH_DELIMIT)
	   {
		   dirName += OS_PATH_DELIMIT_STR;
	   }
	   dirName += "*";

	   struct _finddata_t dirAttribs;
	   intptr_t dirHandle;   
	   dirHandle = _findfirst(dirName.c_str(), &dirAttribs);
	   if(dirHandle == -1)
	   {	   
		   return false;
	   }
	   else
	   {
		   _findclose(dirHandle);
		   return true;
	   }
	}

#else
    bool Disk::DoesDirExist(string dirName)
	{
		DIR *dirP = opendir(dirName.c_str());
		if(dirP == NULL)
			return false;
		else
		{
			closedir(dirP);
			return true;
		}
	}

	void Disk::MatchesInPattern(string pattern, vector<string> &fileNames)
	{
		ENSURE(fileNames.size() == 0);

		glob_t globbuf;
		globbuf.gl_offs = 0;
		int status = glob(pattern.c_str(), GLOB_DOOFFS | GLOB_ERR | GLOB_MARK, NULL, &globbuf);

		if(status == 0)
		{
			for(int iFile = 0; iFile < globbuf.gl_pathc; iFile++)
			{
				string newFile = globbuf.gl_pathv[iFile];
				fileNames.push_back(newFile);
			}
		}

		globfree(&globbuf);
	}
#endif

