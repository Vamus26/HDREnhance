#include "Segmenter.h"
#include "tdef.h"

Segmenter::SegmenterParams Segmenter::SegmenterParams::Default()
{
	Segmenter::SegmenterParams params;

	params.spatialRadius = 2;
	params.colorRadius   = 1.0f;

	params.smoothImgSpaceD = 1.0f;
	params.smoothImgSigD   = 0.04f;
	params.smoothImgKernelHW = 2;

	params.speedUp       = NO_SPEEDUP;
	//params.speedUp       = HIGH_SPEEDUP; //check
	params.smoothingLevel = 3;
	params.segBreakerParams = SegmentBreaker::SegmentBreakerParams::Default();

	return params;
}

void Segmenter::SegmenterParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sSegmenter params:\n", prefixPtr);
	printf("%s  spatialRadius     %i:\n", prefixPtr, spatialRadius);
	printf("%s  colorRadius       %f:\n", prefixPtr, colorRadius);
	printf("%s  speedUp           %i:\n", prefixPtr, (int) speedUp);
	printf("%s  smoothingLevel    %i:\n", prefixPtr, smoothingLevel);
	printf("%s  smoothImgSpaceD   %f:\n", prefixPtr, smoothImgSpaceD);
	printf("%s  smoothImgSigD     %f:\n", prefixPtr, smoothImgSigD);
	printf("%s  smoothImgKernelHW %i:\n", prefixPtr, smoothImgKernelHW);

	segBreakerParams.Dump(prefix + "  ");
	
	printf("\n");
}
