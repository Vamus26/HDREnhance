#include "Flow.h"

Flow::FlowParams Flow::FlowParams::Default()
{
	Flow::FlowParams params;

	params.processInMem = false;
	params.searchWinSize = 10;
	params.noiseVar = 0.003f;
	params.maxDataCost = 0.2f;

	return params;
}

void Flow::FlowParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sFlow params:\n", prefixPtr);
	printf("%s  processInMem: %i\n", prefixPtr, processInMem);
	printf("%s  searchWinSize: %i\n", prefixPtr, searchWinSize);
	printf("%s  noiseVar: %f\n", prefixPtr, noiseVar);
	printf("%s  maxDataCost: %f\n", prefixPtr, maxDataCost);
	printf("\n");
}