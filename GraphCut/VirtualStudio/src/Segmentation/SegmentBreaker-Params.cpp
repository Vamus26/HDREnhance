#include "SegmentBreaker.h"

SegmentBreaker::SegmentBreakerParams SegmentBreaker::SegmentBreakerParams::Default()
{
	SegmentBreaker::SegmentBreakerParams params;

	params.breakGridSize = 8;
	params.minSegSize    = 20;
	params.maxSegSize    = (params.breakGridSize * params.breakGridSize) * 3;

	return params;
}

void SegmentBreaker::SegmentBreakerParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sSegmentBreaker params:\n", prefixPtr);
	printf("%s  breakGridSize: %i\n", prefixPtr, breakGridSize);
	printf("%s  maxSegSize:    %i\n", prefixPtr, maxSegSize);
	printf("%s  minSegSize:    %i\n", prefixPtr, minSegSize);
	printf("\n");
}
