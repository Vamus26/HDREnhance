#include "VideoEnhancer.h"


VideoEnhancer::SuperResParams VideoEnhancer::SuperResParams::Default()
{
	SuperResParams params;

	params.blurKernelStdDev = 2.0f;
	params.downSampleScale  = 4;
	params.dogKernelStdDev  = 7.0f;

	return params;
}


VideoEnhancer::VideoEnhancerParams VideoEnhancer::VideoEnhancerParams::Default()
{
	VideoEnhancerParams params;

	params.superResParams = VideoEnhancer::SuperResParams::Default();

	params.sPatchSize = 5;
	params.lPatchSize = 15;
	params.sPatchOverlap = 1;
	
	params.maxLabels = 16;
	params.trainingDataSF = 1;
	params.annErrorBound = 0.0;	
	params.matchKernelCornerWeight = 0.25f;

	params.matchEpsilon = 0.1f;
	params.renderEpsilon = 0.1f;
	
	params.dataCostNoiseStdDev = 0.075f;
	params.spatialCostNoiseStdDev = 0.2f;

	params.enhancementMode = EM_SUPER_RES;	

	params.bpIters = 20;

	params.outPrefix = "";

	return params;
}

void VideoEnhancer::VideoEnhancerParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sVideo Enhancement params:\n", prefixPtr);

	printf("\n");
}

void VideoEnhancer::SuperResParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sSuper resolution params:\n", prefixPtr);

	printf("\n");
}
