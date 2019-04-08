#include "SuperRes.h"


SuperRes::SuperResParams SuperRes::SuperResParams::Default()
{
	SuperResParams params;

	params.blurKernelStdDev = 2.0f;
	params.patchSize = 5;
	params.largePatchSize = 15;
	params.downSampleScale = 5;
	params.maxLabels = 16;
	params.annErrorBound = 0.0;

	params.dogKernelStdDev = 5.0f;
	
	params.dataCostNoiseStdDev = 0.075f;
	params.spatialCostNoiseStdDev = 0.3f;

	params.normalizeMeanStdDev = false;

	params.patchOverlap = 1;

	params.bpIters = 6;

	params.outPrefix = "";

	return params;
}

void SuperRes::SuperResParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sSuperRes params:\n", prefixPtr);

	printf("\n");
}
