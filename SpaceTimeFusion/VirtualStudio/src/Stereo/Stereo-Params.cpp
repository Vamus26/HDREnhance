#include "Stereo.h"

Stereo::StereoParams Stereo::StereoParams::Default()
{
	Stereo::StereoParams params;

	params.noiseVar = 0.0035f;
	params.dataCostScale = 1.0f;
	params.dataCostExp = 4.0f;
	params.bpIters = 500;
	params.processInMem = false;
	params.spatialKernelSize = 8;
	params.pixelDistBetweenFrontoPlanes = 1.0f;
	params.pcLowerBound = CVector3(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	params.pcUpperBound = CVector3( DBL_MAX,  DBL_MAX,  DBL_MAX);
	params.onlyProcessPhotos = false;
	params.onlyUseScenePlanes = false;

	params.segmenterParams = Segmenter::SegmenterParams::Default();
	params.pcsParams       = PCloudSegmenter::PCloudSegmenterParams::Default();

	return params;
}

void Stereo::StereoParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sStereo params:\n", prefixPtr);
	printf("%s  pixelDistBetweenFrontoPlanes - %f\n", prefixPtr, pixelDistBetweenFrontoPlanes);
	printf("%s  noiseVar                     - %f\n", prefixPtr, noiseVar);
	printf("%s  dataCostScale                - %f\n", prefixPtr, dataCostScale);
	printf("%s  spatialKernelSize            - %i\n", prefixPtr, spatialKernelSize);
	printf("%s  bpIters                      - %i\n", prefixPtr, bpIters);
	printf("%s  processInMem                 - %i\n", prefixPtr, processInMem);
	printf("%s  onlyProcessPhotos            - %i\n", prefixPtr, onlyProcessPhotos);
	printf("%s  onlyUseScenePlanes           - %i\n", prefixPtr, onlyUseScenePlanes);

	if(pcLowerBound[0] != -DBL_MAX)
		printf("%s  pc lower bound               - [%lf, %lf, %lf]\n", prefixPtr, pcLowerBound[0], pcLowerBound[1], pcLowerBound[2]);
	else
	{
		INSIST(pcLowerBound[1] == -DBL_MAX); INSIST(pcLowerBound[2] == -DBL_MAX);
		printf("%s  pc lower bound               - [-DBL_MAX, -DBL_MAX, -DBL_MAX]\n");
	}

	if(pcUpperBound[0] != DBL_MAX)	
		printf("%s  pc upper bound               - [%lf, %lf, %lf]\n", prefixPtr, pcUpperBound[0], pcUpperBound[1], pcUpperBound[2]);
	else
	{
		INSIST(pcUpperBound[1] == DBL_MAX); INSIST(pcUpperBound[2] == DBL_MAX);
		printf("%s  pc lower bound               - [DBL_MAX, DBL_MAX, DBL_MAX]\n");
	}
	
	segmenterParams.Dump(prefix + "  ");
	pcsParams.Dump(prefix + "  ");

	printf("\n");
}
