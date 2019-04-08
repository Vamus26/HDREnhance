#ifndef __FEATURE_RENDERER_H__
#define __FEATURE_RENDERER_H__

#include "Image.h"
#include "FeatureDefs.h"
#include "Features.h"
#include "Transform-3x3.h"

class FeatureRenderer
{
public:
    
	static CFloatImage RenderFeatureMatches(CFloatImage &sourceImg, CFloatImage &targetImg, 
											  FeatureSet &sourceFeatures, FeatureSet &targetFeatures,
											  FeatureMatches &matches, FeatureMatchSet &inliers,
											  float renderProb = 1.0f);

	static CFloatImage RenderFeatureMatches(CFloatImage &sourceImg, CFloatImage &targetImg, 
												  FeatureSet &sourceFeatures, FeatureSet &targetFeatures,
												  FeatureMatches &matches,
												  float renderProb = 1.0f);

	static void RenderFeatureSet(CFloatImage &img, FeatureSet &features,
							     float renderProb = 1.0f);

	static void DrawLine(CFloatImage &img, int x1, int y1, int x2, int y2, CVector3 color);

	static void DrawPoint(CFloatImage &img, int x, int y, int radius, CVector3 color);

	static CVector3 GetRandomColor();


	static CFloatImage RenderEpipolarLineMatches(CFloatImage &sourceImage, CFloatImage &targetImage, 
													const FeatureSet &sourceFeatures, 
													const FeatureSet &targetFeatures,
													const FeatureMatches &matches, 
													const CTransform3x3 &fundamentalMatrix,
													float renderProb = 1.0f);

	static void RenderEpipolarLine(const Feature &sourceFea, const Feature &targetFea,
									 CFloatImage &sourceImage, CFloatImage &targetImage, 
									 const CTransform3x3 &fundamentalMatrix);
};

#endif
