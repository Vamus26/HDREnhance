#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "Debug.h"

void DebugHelper::DUMP(char *formatStr, ...)
{
	va_list		argList;
	char		outBuffer[1024];
	
	va_start(argList, formatStr);
	vsprintf(outBuffer, formatStr, argList);
	va_end(argList);

	fprintf(stdout, "%s", outBuffer);
}

void DebugHelper::DUMP_AND_EXIT(char *formatStr, ...)
{
	va_list		argList;
	char		outBuffer[1024];
	
	va_start(argList, formatStr);
	vsprintf(outBuffer, formatStr, argList);
	va_end(argList);

	fprintf(stdout, "%s\n", outBuffer);

	exit(EXIT_FAILURE);
}


