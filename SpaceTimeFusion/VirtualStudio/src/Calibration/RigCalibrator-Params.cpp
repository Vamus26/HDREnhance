#include "RigCalibrator.h"

RigCalibrator::RigCalibratorParams RigCalibrator::RigCalibratorParams::Default()
{
	RigCalibrator::RigCalibratorParams params;
	params.frameSeqToSearch.startIndex = params.frameSeqToSearch.endIndex = -1;
	params.framesToFind = 10;
	params.maxSharpenIters = 3;
	params.useNoahCalib = false;
	params.useDrewCalib = false;
	params.useLarryCalib = false;
	params.minViewsForScenePoint = 3;
	params.pcLowerBound = CVector3(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	params.pcUpperBound = CVector3( DBL_MAX,  DBL_MAX,  DBL_MAX);
	return params;
}

void RigCalibrator::RigCalibratorParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sRig calibrator params:\n", prefixPtr);

	if(useNoahCalib || useLarryCalib)
	{
		ENSURE(useNoahCalib != useLarryCalib);
		if(useNoahCalib || useDrewCalib)
		{
			ENSURE(useNoahCalib != useDrewCalib);
			printf("%s  useNoahCalib          - %i\n", prefixPtr, useNoahCalib);
			printf("%s  useDrewCalib          - %i\n", prefixPtr, useDrewCalib);
			printf("%s  minViewsForScenePoint - %i\n", prefixPtr, minViewsForScenePoint);

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
		}
		if(useLarryCalib)
			printf("%s  useLarryCalib - true\n", prefixPtr);
		printf("%s  calibFN  - %s\n", prefixPtr, calibFN.c_str());
	}
	else
	{
		printf("%s  [startSearchIndex, endSearchIndex] - [%i, %i]:\n", 
				prefixPtr, frameSeqToSearch.startIndex, frameSeqToSearch.endIndex);

		printf("%s  framesToFind    - %i\n", prefixPtr, framesToFind);
		printf("%s  sharpenAmount   - %i\n", prefixPtr, sharpenAmount);
		printf("%s  maxSharpenIters - %i\n", prefixPtr, maxSharpenIters);
		printf("%s  refCamID        - %s\n", prefixPtr, refCamID.c_str());
	}

	printf("\n");
}
