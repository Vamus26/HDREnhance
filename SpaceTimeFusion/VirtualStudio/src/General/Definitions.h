#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

//bug
//#define __GROTTO__
//#define __FLOWERS__
//#define __PC__
#define __SUZZALO__
//#define __DT__
//#define __FKY__
//#define __PF__
//#define __ST__
//#define __TEST__

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <iostream>
#pragma warning(disable: 4702)
#pragma warning(disable: 4204)
#include <vector>
using namespace std;
#include "Debug.h"

#ifndef uchar 
typedef unsigned char uchar;
#endif

const uchar MASK_VALID   = UCHAR_MAX;
const uchar MASK_INVALID = 0;

#ifndef uint
typedef unsigned int uint;
#endif

#define SQRT_2 1.41421356237309504880f

typedef std::pair<int, int> IntPair;

typedef struct _FrameSeq
{
	int startIndex;
	int endIndex;
} FrameSeq;
#define CHECK_FS_ORDER(x) ENSURE(x.startIndex <= x.endIndex);
#define CHECK_FS_RANGE(x, minIndex, maxIndex) {					                               \
												CHECK_FS_ORDER(x);                             \
												CHECK_RANGE(x.startIndex, minIndex, maxIndex); \
											    CHECK_RANGE(x.endIndex,   minIndex, maxIndex); \
										      }

#define SET_IF_LT(var, val) if(val < var) var = val;

#define SET_IF_GT(var, val) if(val > var) var = val;

#define SQR(x) ((x) * (x))

inline uint NEAREST_UINT(float n)
{
	ENSURE(n >= 0.0f);
	return (uint) (n + 0.5f);
}

inline int NEAREST_INT(float n)
{
	return (int) (n + 0.5f);
}

inline int NEAREST_INT(double n)
{
	return (int) (n + 0.5);
}

template <class T>
inline bool InRange(T x, T min, T max)
{
	return ((x >= min) && (x <= max)) ? true : false;
}

#ifdef WIN32
	#include <direct.h>
	#include <typeinfo.h>
	#include <io.h>

	const int MY_RAND_MAX = ((RAND_MAX << 15) + RAND_MAX);
	inline int myRand()
	{
		return ((rand() << 15) + rand());
	}

	#include <hash_set>
	#include <hash_map>
	using namespace stdext;

	#define COPY_CMD   "copy"
	#define RENAME_CMD "ren"	

	const char   OS_PATH_DELIMIT     = '\\';
	const string OS_PATH_DELIMIT_STR = "\\";
	const char WRONG_OS_PATH_DELIMIT = '/';

	typedef _int64 AccurateTime;

	#define MKDIR(dirName, permissions) mkdir(dirName)

#else

	#include <sys/io.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <typeinfo>
	#include <glob.h>
	#include <dirent.h>

    const int MY_RAND_MAX = RAND_MAX;
	inline int myRand()
	{
		return rand();
	}

	#include <ext/hash_set>
	#include <ext/hash_map>
	using namespace __gnu_cxx;

	#define COPY_CMD   "cp"
	#define RENAME_CMD "mv"	

	const char OS_PATH_DELIMIT       = '/';
	const string OS_PATH_DELIMIT_STR = "/";
	const char WRONG_OS_PATH_DELIMIT = '\\';

	typedef signed long long AccurateTime;

	#define MKDIR(dirName, permissions) mkdir(dirName, permissions)

#endif

inline float genProb()
{
	return ((float) myRand() / MY_RAND_MAX);
}

void Tokenize(const string& str,
              vector<string>& tokens,
              const string& delimiters = " \t");

bool SilentSysCall(const char *cmd, 
				   bool silenceStdOut = true, 
				   bool silenceStdErr = true);


#endif


