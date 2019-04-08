#ifndef __SAMPLE_TEST_H__
#define __SAMPLE_TEST_H__
#include "Definitions.h"
#include "ImageIO.h"

class SampleTest
{
	CFloatImage hiRes1;
	CFloatImage hiRes2;
	CFloatImage lowRes1;
	CFloatImage lowRes2;

	CFloatImage currPixelSampleProb;


public:
	SampleTest(CFloatImage hiRes1, CFloatImage hiRes2);

	void Resample(int searchWinSize);

private:
	void computePixelSampleProbImg(float x, float y, float searchWinSize);
	float computePixelSampleProbImg(float x, float y, float xx, float yy, int channel);

	void normalizePixelProbImg(CFloatImage &probImg);

	void discardLowProbs(CFloatImage &probImg, int probsToSave);

	void resample(CFloatImage &dstImg, const CFloatImage &srcImg, int dstX, int dstY, 
			      CFloatImage &sampleProbImg, int searchWinSize, float scale);

	void fillInGaps(CFloatImage &dstImg);

	void scaleProbImg(CFloatImage &srcProbImg, CFloatImage &dstProbImg);

};

#endif