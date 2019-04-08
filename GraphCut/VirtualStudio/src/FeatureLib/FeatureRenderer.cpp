#include "FeatureRenderer.h"
#include "Definitions.h"
#include "Debug.h"
#include "time.h"
#include "Motion.h"
#include<algorithm>
CFloatImage FeatureRenderer::RenderFeatureMatches(CFloatImage &sourceImg, CFloatImage &targetImg, 
												  FeatureSet &sourceFeatures, FeatureSet &targetFeatures,
												  FeatureMatches &matches, FeatureMatchSet &inliers,
												  float renderProb)
{

	FeatureMatches inlierMatches;

	for(uint iInlier = 0; iInlier < inliers.size(); iInlier++)
	{

		uint matchIndex = inliers[iInlier];
		inlierMatches.push_back(matches[matchIndex]);
	}

	return RenderFeatureMatches(sourceImg, targetImg, 
							    sourceFeatures, targetFeatures,
								inlierMatches, renderProb);
}



CFloatImage FeatureRenderer::RenderFeatureMatches(CFloatImage &sourceImg, CFloatImage &targetImg, 
												  FeatureSet &sourceFeatures, FeatureSet &targetFeatures,
												  FeatureMatches &matches,
												  float renderProb)
{
	CShape sourceShape = sourceImg.Shape();
	CShape targetShape = targetImg.Shape();
	CShape canvasShape(sourceShape.width  + targetShape.width,
					   (std::max)(sourceShape.height, targetShape.height),
					   sourceShape.nBands);
	ENSURE(sourceShape.nBands == targetShape.nBands);

	CFloatImage canvas(canvasShape);
	canvas.ClearPixels();

    uint   nodeAddr  = 0;
	uint   pixelAddr = 0;		
	for(int y = 0; y < sourceShape.height; y++)
	{
		for(int x = 0; x < sourceShape.width; x++, nodeAddr++, pixelAddr += sourceShape.nBands)
		{
			uint targetPixelAddr = canvasShape.PixelIndex(x, y, 0);
			canvas[targetPixelAddr + 0] = sourceImg[pixelAddr + 0];
			if(sourceShape.nBands == 3)
			{
				canvas[targetPixelAddr + 1] = sourceImg[pixelAddr + 1];
				canvas[targetPixelAddr + 2] = sourceImg[pixelAddr + 2];
			}
		}
	}

    nodeAddr  = 0;
	pixelAddr = 0;		
	for(int y = 0; y < targetShape.height; y++)
	{
		for(int x = 0; x < targetShape.width; x++, nodeAddr++, pixelAddr += targetShape.nBands)
		{
			uint targetPixelAddr = canvasShape.PixelIndex(x + sourceShape.width , y, 0);
			canvas[targetPixelAddr + 0] = targetImg[pixelAddr + 0];
			if(sourceShape.nBands == 3)
			{
				canvas[targetPixelAddr + 1] = targetImg[pixelAddr + 1];
				canvas[targetPixelAddr + 2] = targetImg[pixelAddr + 2];
			}
		}
	}

	for(uint iMatch = 0; iMatch < matches.size(); iMatch++)
	{
		if(genProb() <= renderProb)
		{
			int sourceIndex = matches[iMatch].first;
			int targetIndex = matches[iMatch].second;
			
			int sourceX = NEAREST_INT(sourceFeatures[sourceIndex].x);
			int sourceY = NEAREST_INT(sourceFeatures[sourceIndex].y);


			int targetX = NEAREST_INT(targetFeatures[targetIndex].x) + sourceShape.width;
			int targetY = NEAREST_INT(targetFeatures[targetIndex].y);

			//CVector3 color = FeatureRenderer::GetRandomColor();

			CVector3 color(0, 0, 1);

			FeatureRenderer::DrawPoint(canvas, sourceX, sourceY, 2, color);
			FeatureRenderer::DrawPoint(canvas, targetX, targetY, 2, color);

			FeatureRenderer::DrawLine(canvas, 
									  sourceX, sourceY,
									  targetX, targetY,
									  color);
		}
	}

	return canvas;
}


void FeatureRenderer::RenderFeatureSet(CFloatImage &img, FeatureSet &features, float renderProb)
{
	CShape imgShape = img.Shape();

	int featureTypeCount = Feature::FeatureTypeCount;
	ENSURE(featureTypeCount == 4);
	CVector3 feaColors[Feature::FeatureTypeCount];
	feaColors[0] = CVector3(1, 0, 0);
	feaColors[1] = CVector3(0, 1, 0);
	feaColors[2] = CVector3(0, 0, 1);
	feaColors[3] = CVector3(0, 1, 1);

	for(FeatureSet::const_iterator currFeature = features.begin();
		currFeature != features.end();
		currFeature++)
	{
		if(genProb() <= renderProb)
		{
			int imgX = NEAREST_INT(currFeature->x);
			int imgY = NEAREST_INT(currFeature->y);
			ENSURE(imgShape.InBounds(imgX, imgY));
			ENSURE(currFeature->type < Feature::FeatureTypeCount);
			DrawPoint(img, imgX, imgY, 2, feaColors[currFeature->type]);
		}
	}
}

CVector3 FeatureRenderer::GetRandomColor()
{
	CVector3 color;
	for(int i = 0; i < 3; i++)
	{
		color[i] = genProb();
		color[i] = (color[i] / 2.0f) + 0.5f;
	}

	return color;
}


void FeatureRenderer::DrawLine(CFloatImage &img, int x1, int y1, int x2, int y2, CVector3 col)
{
	CShape imgShape = img.Shape();

	int xSpan = abs(x2 - x1);
	int ySpan = abs(y2 - y1);
	int span;

	ENSURE((xSpan != 0) || (ySpan != 0));

	float xIncr = 1.0f;
	float yIncr = 1.0f;

	if(xSpan > ySpan)
	{
		span  = xSpan;
		yIncr = ((float) ySpan / xSpan);
	}
	else
	{
		span = ySpan;
		xIncr = ((float) xSpan / ySpan);
	}

	float x = (float) x1;
	float y = (float) y1;
	if(x2 < x1) xIncr *= -1.0f;
	if(y2 < y1) yIncr *= -1.0f;
	for(int i = 0; i < span; i++)
	{		
		int xx = NEAREST_INT(x);
		int yy = NEAREST_INT(y);
		if(imgShape.InBounds(xx, yy) == true)
		{
			//DrawPoint(img, xx, yy, 2, col);
			img.Pixel(xx, yy, 0) = (float) col[0];
			if(imgShape.nBands == 3)
			{
				img.Pixel(xx, yy, 1) = (float) col[1];
				img.Pixel(xx, yy, 2) = (float) col[2];
			}
		}
		x += xIncr;
		y += yIncr;
	}
}

void FeatureRenderer::DrawPoint(CFloatImage &img, int x, int y, int radius, CVector3 col)
{
	CShape imgShape = img.Shape();

	int radiusSqr = radius * radius;
	for(int yy = y - radius, yDist = -radius; yy <= y + radius; yy++, yDist++)
	{
		for(int xx = x - radius, xDist = -radius; xx <= x + radius; xx++, xDist++)
		{
			if(imgShape.InBounds(xx, yy) == true)
			{
				if(((xDist * xDist) + (yDist * yDist)) <= radiusSqr)
				{
					img.Pixel(xx, yy, 0) = (float) col[0];
					if(imgShape.nBands == 3)
					{
						img.Pixel(xx, yy, 1) = (float) col[1];
						img.Pixel(xx, yy, 2) = (float) col[2];
					}
				}
			}
		}
	}
}


CFloatImage FeatureRenderer::RenderEpipolarLineMatches(CFloatImage &sourceImage, CFloatImage &targetImage, 
														const FeatureSet &sourceFeatures, 
														const FeatureSet &targetFeatures,
														const FeatureMatches &matches, 
														const CTransform3x3 &fundamentalMatrix,
														float renderProb)
{
	for(uint iMatch = 0; iMatch < matches.size(); iMatch++)
	{
		if(genProb() <= renderProb)
		{
			int sourceIndex = matches[iMatch].first;
			int targetIndex = matches[iMatch].second;
			RenderEpipolarLine(sourceFeatures[sourceIndex], targetFeatures[targetIndex], 
							   sourceImage, targetImage, fundamentalMatrix);		
		}
	}

	CShape sourceShape = sourceImage.Shape();
	CShape targetShape = targetImage.Shape();

	CShape canvasShape(sourceShape.width  + targetShape.width,
					   max(sourceShape.height, targetShape.height),
					   sourceShape.nBands);
	ENSURE(sourceShape.nBands == targetShape.nBands);

	CFloatImage canvas(canvasShape);
	canvas.ClearPixels();

    uint   nodeAddr  = 0;
	uint   pixelAddr = 0;		
	for(int y = 0; y < sourceShape.height; y++)
	{
		for(int x = 0; x < sourceShape.width; x++, nodeAddr++, pixelAddr += sourceShape.nBands)
		{
			uint targetPixelAddr = canvasShape.PixelIndex(x, y, 0);
			canvas[targetPixelAddr + 0] = sourceImage[pixelAddr + 0];
			if(sourceShape.nBands == 3)
			{
				canvas[targetPixelAddr + 1] = sourceImage[pixelAddr + 1];
				canvas[targetPixelAddr + 2] = sourceImage[pixelAddr + 2];
			}
		}
	}

    nodeAddr  = 0;
	pixelAddr = 0;		
	for(int y = 0; y < targetShape.height; y++)
	{
		for(int x = 0; x < targetShape.width; x++, nodeAddr++, pixelAddr += targetShape.nBands)
		{
			uint targetPixelAddr = canvasShape.PixelIndex(x + sourceShape.width , y, 0);
			canvas[targetPixelAddr + 0] = targetImage[pixelAddr + 0];
			if(sourceShape.nBands == 3)
			{
				canvas[targetPixelAddr + 1] = targetImage[pixelAddr + 1];
				canvas[targetPixelAddr + 2] = targetImage[pixelAddr + 2];
			}
		}
	}

	return canvas;
}


void FeatureRenderer::RenderEpipolarLine(const Feature &sourceFea, const Feature &targetFea,
										 CFloatImage &sourceImage, CFloatImage &targetImage, 
										 const CTransform3x3 &fundamentalMatrix)
{	
	const static int LineHalfLength = 10;

	CVector3 sourceLine = Motion::GetEpiLine(fundamentalMatrix.Transpose(), targetFea);
	CVector3 sourceLineDirc = Motion::GetEpiLineDirc(sourceLine);

	CVector3 targetLine = Motion::GetEpiLine(fundamentalMatrix, sourceFea);
	CVector3 targetLineDirc = Motion::GetEpiLineDirc(targetLine);
	
	CVector3 color = FeatureRenderer::GetRandomColor();
	DrawPoint(sourceImage, NEAREST_INT(sourceFea.x), NEAREST_INT(sourceFea.y), 3, color);
	DrawPoint(targetImage, NEAREST_INT(targetFea.x), NEAREST_INT(targetFea.y), 3, color);

	int x1, y1, x2, y2;

	x1 = NEAREST_INT((float) (targetFea.x - (LineHalfLength * targetLineDirc[0])));
	y1 = NEAREST_INT((float) (targetFea.y - (LineHalfLength * targetLineDirc[1])));
	x2 = NEAREST_INT((float) (targetFea.x + (LineHalfLength * targetLineDirc[0])));
	y2 = NEAREST_INT((float) (targetFea.y + (LineHalfLength * targetLineDirc[1])));
	FeatureRenderer::DrawLine(targetImage, x1, y1, x2, y2, color);

	x1 = NEAREST_INT((float) (sourceFea.x - (LineHalfLength * sourceLineDirc[0])));
	y1 = NEAREST_INT((float) (sourceFea.y - (LineHalfLength * sourceLineDirc[1])));
	x2 = NEAREST_INT((float) (sourceFea.x + (LineHalfLength * sourceLineDirc[0])));
	y2 = NEAREST_INT((float) (sourceFea.y + (LineHalfLength * sourceLineDirc[1])));
	FeatureRenderer::DrawLine(sourceImage, x1, y1, x2, y2, color);
}
