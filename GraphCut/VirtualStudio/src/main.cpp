#pragma warning(disable : 4099)
#pragma warning(disable : 4224)
//#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib, "wsock32.lib")

#include <stdlib.h>
#include <stdio.h>
#include "Disk.h"
#include "Timer.h"
#include "OptFileParser.h"
#include "Driver.h"
#include <math.h>
#include "ImageIO.h"
#include "FlowHelper.h"

/*
#include "VStudioWin.h"
#include "VideoEnhancer.h"
#include "ImageProcessing.h"
*/

///*
int main(int args, char *argv[])
{
	int exitCode = EXIT_SUCCESS;

	if(args < 2)
	{
		printf("[usage] %s options-filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	OptFileParser optionsParser;
	OptFileParser::OptFileParams params = optionsParser.Load(argv[1], argv + 2);
	params.Dump();

	Driver driver(params);
	driver.Start();

	//VStudioWin vsWin;
	//exitCode = vsWin.Start(params);

	return exitCode;
}
//*/



extern "C"
{
	void _check_commonlanguageruntime_version() {}
} 

