#ifndef __SEGMENTER_H__
#define __SEGMENTER_H__

#include "Definitions.h"
#include "Image.h"
#include "tdef.h"
#include "Segmentation.h"
#include "SegmentBreaker.h"
#include "Camera.h"

class Segmenter
{
public:
	typedef struct _SegmenterParams
	{
		int   spatialRadius;
		float colorRadius;
		SpeedUpLevel  speedUp;
		int smoothingLevel;

		float smoothImgSpaceD;
		float smoothImgSigD;
		int smoothImgKernelHW;

		SegmentBreaker::SegmentBreakerParams segBreakerParams;
		
		static _SegmenterParams Default();
		void Dump(string prefix);
	} SegmenterParams; 

private:
	SegmenterParams params;

public: //Segmenter.cpp
	void CreateSegmentation(const Camera &camera, 
							int iFrame, 
							Segmentation &segmentation, SegmenterParams sParams);

private: //Segmenter.cpp

	void createSegmentation(const CByteImage &img, Segmentation &segmentation);
	int createLabelData(const CByteImage &img, Segmentation &segmentation);
	void breakLargeSegments(Segmentation &segmentation);
};

#endif
