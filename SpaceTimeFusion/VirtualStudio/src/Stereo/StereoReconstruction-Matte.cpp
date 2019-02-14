#include "StereoReconstruction.h"
#include "ImageIO.h"
#include "ImageProcessing.h"
#include "PiecesBlender.h"
#include "CCLabeler.h"
#include "BlendSolver.h"
#include "FlowHelper.h"
#include "Draw.h"
#include "Disk.h"
#include "Timer.h"
#include "defines.h"


void StereoReconstruction::computeMatte(SRS_Context &srsContext, SR_Params srParams)
{
	CShape srsMaskShape = srsContext.sourceMask.Shape();
	CShape srsImgShape  = srsContext.sourceImg.Shape();
	INSIST(srsImgShape.SameIgnoringNBands(srsMaskShape));
	INSIST(srsMaskShape.nBands == 1);

	CShortImage fgFrontoPlaneMap(srsMaskShape);
	CShortImage bgFrontoPlaneMap(srsMaskShape);
	CShortImage srsFrontoPlaneMap(srsMaskShape);

	ImageProcessing::Operate(fgFrontoPlaneMap,  fgFrontoPlaneMap,  (short) SEGMENT_OccDepPlaneIndex, ImageProcessing::OP_SET);	
	ImageProcessing::Operate(bgFrontoPlaneMap,  bgFrontoPlaneMap,  (short) SEGMENT_OccDepPlaneIndex, ImageProcessing::OP_SET);	
	ImageProcessing::Operate(srsFrontoPlaneMap, srsFrontoPlaneMap, (short) SEGMENT_OccDepPlaneIndex, ImageProcessing::OP_SET);	

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < srsMaskShape.height; y++)
	for(int x = 0; x < srsMaskShape.width;  x++, nodeAddr++, pixelAddr += srsImgShape.nBands)
	{
		int srsPlaneIndex = srsContext.sourcePlaneMap[nodeAddr];
		srsFrontoPlaneMap[nodeAddr] = getFrontoPlaneIndex(x, y, srsContext, srsPlaneIndex, false);
	}


	CFloatImage fgImg = srsContext.sourceImg.Clone();;	
	CFloatImage bgImg(srsImgShape);
	bgImg.ClearPixels();

	CFloatImage imgAlpha(srsImgShape);
	ImageProcessing::Operate(imgAlpha, imgAlpha, 1.0f, ImageProcessing::OP_SET);

	INSIST(srParams.mattingRad % 2 == 1);
	int neighAreaElemCount       = SQ((2 * srParams.mattingRad) + 1);
	short *neighFrontoPlanes     = new short[neighAreaElemCount];
	short *neighFrontoPlaneCount = new short[neighAreaElemCount];
	INSIST(neighFrontoPlanes != NULL);
	INSIST(neighFrontoPlaneCount != NULL);

	nodeAddr  = 0;
	pixelAddr = 0;
	for(int y = 0; y < srsMaskShape.height; y++)
	for(int x = 0; x < srsMaskShape.width;  x++, nodeAddr++, pixelAddr += srsImgShape.nBands)
	{		
		int fgFrontoPlaneIndex = srsFrontoPlaneMap[nodeAddr];
		if(fgFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) continue;

		fgFrontoPlaneMap[nodeAddr] = fgFrontoPlaneIndex;
		bgFrontoPlaneMap[nodeAddr] = SEGMENT_OccDepPlaneIndex;

		memset(neighFrontoPlaneCount, 0, sizeof(*neighFrontoPlaneCount) * neighAreaElemCount);
		for(int yShift = -srParams.mattingRad; yShift <= srParams.mattingRad; yShift++)
		for(int xShift = -srParams.mattingRad; xShift <= srParams.mattingRad; xShift++)
		{
			int xx = x + xShift;
			int yy = y + yShift;
			if(srsMaskShape.InBounds(xx, yy) == false) continue;

			int neighFrontoPlaneIndex = srsFrontoPlaneMap.Pixel(xx, yy, 0);
			if((neighFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) ||
			   (abs(neighFrontoPlaneIndex - fgFrontoPlaneIndex) < srParams.depthMatchThres))
			{
				continue;
			}

			for(int iNeigh = 0; iNeigh < neighAreaElemCount; iNeigh++)
			{
				if(neighFrontoPlaneCount[iNeigh] == 0)
				{
					neighFrontoPlaneCount[iNeigh] = 1;
					neighFrontoPlanes[iNeigh] = neighFrontoPlaneIndex;
					break;
				}
				else if(abs(neighFrontoPlanes[iNeigh] - neighFrontoPlaneIndex) < srParams.depthMatchThres)
				{
					neighFrontoPlaneCount[iNeigh]++;
					break;
				}
			}
		}

		int bgFrontoPlaneSupport = 0;
		short bgFrontoPlaneIndex = SEGMENT_OccDepPlaneIndex;
		for(int iNeigh = 0; iNeigh < neighAreaElemCount; iNeigh++)
		{
			if(neighFrontoPlaneCount[iNeigh] == 0) break;

			if(neighFrontoPlaneCount[iNeigh] > bgFrontoPlaneSupport)
			{
				bgFrontoPlaneSupport = neighFrontoPlaneCount[iNeigh];
				bgFrontoPlaneIndex = neighFrontoPlanes[iNeigh];
			}
		}
		bgFrontoPlaneMap[nodeAddr] = bgFrontoPlaneIndex;

		if(bgFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) continue;

		for(int yShift = -srParams.mattingRad; yShift <= srParams.mattingRad; yShift++)
		for(int xShift = -srParams.mattingRad; xShift <= srParams.mattingRad; xShift++)
		{
			int xx = x + xShift;
			int yy = y + yShift;
			if(srsMaskShape.InBounds(xx, yy) == false) continue;

			if(abs(bgFrontoPlaneIndex - srsFrontoPlaneMap.Pixel(xx, yy, 0)) <= srParams.depthMatchThres)
			{
				for(int iBand = 0; iBand < srsImgShape.nBands; iBand++)
				{
					bgImg[pixelAddr + iBand] += srsContext.sourceImg.Pixel(xx, yy, iBand);
				}
			}
		}

		for(int iBand = 0; iBand < srsImgShape.nBands; iBand++)
		{
			bgImg[pixelAddr + iBand] /= bgFrontoPlaneSupport;
		}

		if(fgFrontoPlaneIndex > bgFrontoPlaneIndex)
		{
			bgFrontoPlaneMap[nodeAddr] = fgFrontoPlaneIndex;
			fgFrontoPlaneMap[nodeAddr] = bgFrontoPlaneIndex;

			for(int iBand = 0; iBand < srsImgShape.nBands; iBand++)
			{
				float channelVal = bgImg[pixelAddr + iBand];
				bgImg[pixelAddr + iBand] = fgImg[pixelAddr + iBand];
				fgImg[pixelAddr + iBand] = channelVal;

				imgAlpha[pixelAddr + iBand] = 1.0f - imgAlpha[pixelAddr + iBand];
			}
		}
	}

	CFloatImage imgDistToBG(srsMaskShape);
	ImageProcessing::Operate(imgDistToBG, imgDistToBG, FLT_MAX, ImageProcessing::OP_SET);

	float maxDist = sqrt(2.0f) * srParams.mattingRad;
	nodeAddr  = 0;
	pixelAddr = 0;
	for(int y = 0; y < srsMaskShape.height; y++)
	for(int x = 0; x < srsMaskShape.width;  x++, nodeAddr++, pixelAddr += srsImgShape.nBands)
	{
		if(bgFrontoPlaneMap[nodeAddr] == SEGMENT_OccDepPlaneIndex) continue;

		for(int yShift = -srParams.mattingRad; yShift <= srParams.mattingRad; yShift++)
		for(int xShift = -srParams.mattingRad; xShift <= srParams.mattingRad; xShift++)
		{
			int xx = x + xShift;
			int yy = y + yShift;
			if(srsMaskShape.InBounds(xx, yy) == false) continue;

			float dist = sqrt((float) (yShift * yShift) + (xShift * xShift));
			if(abs(bgFrontoPlaneMap.Pixel(xx, yy, 0) - bgFrontoPlaneMap[nodeAddr]) < srParams.depthMatchThres)
			{
				if(imgDistToBG[nodeAddr] > dist)
				{
					imgDistToBG[nodeAddr] = dist;					
				}
			}
		}

		INSIST(imgDistToBG[nodeAddr] != FLT_MAX);

		for(int iBand = 0; iBand < srsImgShape.nBands; iBand++)
		{
			imgAlpha[pixelAddr + iBand] = min(1.0f, max(0.0f, imgDistToBG[nodeAddr] / maxDist));
		}
	}


	delete [] neighFrontoPlanes;
	delete [] neighFrontoPlaneCount;
}


void StereoReconstruction::computeColorsFB(SRS_Context &srsContext, 
											 CFloatImage &fgImg, CFloatImage &bgImg,
											 CShortImage &fgFrontoPlaneMap, CShortImage &bgFrontoPlaneMap,
											 CFloatImage &imgAlpha, CFloatImage &imgDistToBG,
											 SR_Params srParams)
{
	CShape srsMaskShape = srsContext.sourceMask.Shape();
	CShape srsImgShape  = srsContext.sourceImg.Shape();

	CFloatImage fgImgTemp = fgImg.Clone();
	CFloatImage bgImgTemp = bgImg.Clone();

	fgImg.ClearPixels();
	bgImg.ClearPixels();

	float bgWeight[3]; 
	float fgWeight[3]; 
	INSIST(srsImgShape.nBands < 3);

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < srsMaskShape.height; y++)
	for(int x = 0; x < srsMaskShape.width;  x++, nodeAddr++, pixelAddr += srsImgShape.nBands)
	{	
		short fgFrontoPlaneIndex = fgFrontoPlaneMap[nodeAddr];
		short bgFrontoPlaneIndex = bgFrontoPlaneMap[nodeAddr];
		if(bgFrontoPlaneIndex == SEGMENT_OccDepPlaneIndex) continue;

		bgWeight[0] = bgWeight[1] = bgWeight[2] = 0.0f;
		fgWeight[0] = fgWeight[1] = fgWeight[2] = 0.0f;

		for(int yShift = -srParams.mattingRad; yShift <= srParams.mattingRad; yShift++)
		for(int xShift = -srParams.mattingRad; xShift <= srParams.mattingRad; xShift++)
		{
			int xx = x + xShift;
			int yy = y + yShift;
			if(srsMaskShape.InBounds(xx, yy) == false) continue;

			if(abs(bgFrontoPlaneIndex - bgFrontoPlaneMap.Pixel(xx, yy, 0)) < srParams.depthMatchThres)
			{
				for(int iBand = 0; iBand < srsImgShape.nBands; iBand++)
				{

					bgImg[pixelAddr + iBand] += (1.0f - imgAlpha[nodeAddr]) * bgImgTemp[pixelAddr + iBand];
					bgWeight[iBand] += (1.0f - imgAlpha[nodeAddr]);
				}
			}

			if(abs(bgFrontoPlaneIndex - fgFrontoPlaneMap.Pixel(xx, yy, 0)) < srParams.depthMatchThres)
			{
				for(int iBand = 0; iBand < srsImgShape.nBands; iBand++)
				{

					bgImg[pixelAddr + iBand] += (1.0f - imgAlpha[nodeAddr]) * bgImgTemp[pixelAddr + iBand];
					bgWeight[iBand] += (1.0f - imgAlpha[nodeAddr]);
				}
			}



				//for(k=0;k<3;k++)
				//				{
				//					color0[k] +=
				//						del*(1.0f - alpha[k])*(1.0f - alpha[k])*tmpBColor[(row + rd)*m_Width + col + cd][k];

				//					ct0[k] += del*(1.0f - alpha[k])*(1.0f - alpha[k]);
				//				}
				
		}
	}
}
