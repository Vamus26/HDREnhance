#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <string>

#define PRINT_STR(x)    printf(#x ": %s\n",   (char *)       (x));
#define PRINT_STRING(x) printf(#x ": %s\n",   (char *)       (x).c_str());
#define PRINT_INT(x)    printf(#x ": %i\n",   (int)          (x));
#define PRINT_TS(x)     printf(#x ": %lld\n", (long long int)(x));
#define PRINT_UINT(x)   printf(#x ": %u\n",   (unsigned int) (x));
#define PRINT_FLOAT(x)  printf(#x ": %f\n",   (float)        (x));
#define PRINT_SCI(x)    printf(#x ": %e\n",   (double)       (x));
#define PRINT_DOUBLE(x) printf(#x ": %lf\n",  (double)       (x));
#define TOUCH(x)        ;{x;};

#define DEBUG_DUMP	    printf("[%s, %i]: \n", __FILE__, __LINE__); DebugHelper::DUMP
#define REPORT_FAILURE	printf("[%s, %i]: \n", __FILE__, __LINE__); DebugHelper::DUMP_AND_EXIT
#define REPORT          DebugHelper::DUMP

#define ENSURE(expression)   if((expression) == false)                            \
							 {                                                    \
								 REPORT_FAILURE("ENSURE: %s\n", #expression);     \
							 }

#define INSIST(expression)   if((expression) == false)                            \
							 {                                                    \
							     REPORT_FAILURE("INSIST: %s\n", #expression);     \
							 }

#define CHECK_RANGE(x, min, max) {                        \
								    ENSURE(x >= min);     \
									ENSURE(x <= max);     \
							   	 }

#define INSIST_RANGE(x, min, max) {                       \
								    INSIST(x >= min);     \
									INSIST(x <= max);     \
							   	  }

class DebugHelper
{
public:
	static void DUMP(char *formatStr, ...);
	static void DUMP_AND_EXIT(char *formatStr, ...);
};
#endif


