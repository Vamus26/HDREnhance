#include "Flow.h"
#include "ImageIO.h"
#include "Draw.h"


//void Flow::processCurrCamFlow()
//{
//	vector<Segment> &currViewSegs = this->currView->seg->segments;
//
//
//	//visFlowProjections(449, 218, 0, 0);
//	//exit(0);
//
//	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
//		currSeg != currViewSegs.end();
//		currSeg++)
//	{	
//		currSeg->auxData = (void *) new SegFlowData();
//	}
//
//	for(uint iSeg = 0; iSeg < currViewSegs.size(); iSeg++)
//	{
//		processCurrViewSegFlow(currViewSegs[iSeg]);
//	}
//
//	CFloatImage uVisImg;
//	CFloatImage vVisImg;
//	getCurrViewFlowVis(uVisImg, vVisImg, (float) this->flowParams.searchWinSize);
//	ImageIO::WriteFile(uVisImg, "u.tga");
//	ImageIO::WriteFile(vVisImg, "v.tga");
//	exit(0);
//
//	for(vector<Segment>::iterator currSeg = currViewSegs.begin();
//		currSeg != currViewSegs.end();
//		currSeg++)
//	{
//		SegFlowData *segFlowData = (SegFlowData *) currSeg->auxData;
//		delete segFlowData;
//		currSeg->auxData = NULL;		
//	}
//}

void Flow::processCurrViewSegFlow(Segment &seg)
{
	const CIntImage &labelImg = this->currView->seg->labelImg;
	CShape maskShape = labelImg.Shape();

	float bestOffsetCost = FLT_MIN;
	int bestOffsetX = INT_MAX;
	int bestOffsetY = INT_MAX;

	for(int yOffset = -this->flowParams.searchWinSize; yOffset <= this->flowParams.searchWinSize; yOffset++)
	{
		for(int xOffset = -this->flowParams.searchWinSize; xOffset <= this->flowParams.searchWinSize; xOffset++)
		{
			float currOffsetCost = 0;
			int segPixelsFound = 0;
			for(int y = seg.bounds.minY; y <= seg.bounds.maxY; y++)
			{	
				uint nodeAddr = maskShape.NodeIndex(seg.bounds.minX, y);
				for(int x = seg.bounds.minX; x <= seg.bounds.maxX; x++, nodeAddr++)
				{
					if(labelImg[nodeAddr] == seg.id)
					{
						currOffsetCost += getPixelFlowCost(x, y, xOffset, yOffset, seg);
						segPixelsFound++;
					}
				}
			}
			ENSURE(segPixelsFound == seg.size);

			currOffsetCost /= seg.size;
			if(currOffsetCost > bestOffsetCost)
			{
				bestOffsetCost = currOffsetCost;
				bestOffsetX = xOffset;
				bestOffsetY = yOffset;
			}
		}
	}
	ENSURE(bestOffsetCost != FLT_MAX);

	SegFlowData *segFlowData = (SegFlowData *) seg.auxData;
	segFlowData->xOffset = bestOffsetX;
	segFlowData->yOffset = bestOffsetY;
}

float Flow::getPixelFlowCost(int currX, int currY, int offsetX, int offsetY, const Segment &currViewSeg)
{
	float flowCost = 1.0f;

	int nextX = currX + offsetX;
	int nextY = currY + offsetY;

	CFloatImage currViewImg = this->currView->smoothImg;
	CShape currViewShape = currViewImg.Shape();
	if(currViewShape.InBounds(nextX, nextY) == false)
	{
		return FLOW_OffScreen_Flowcost;
	}

	CFloatImage nextViewImg = this->nextView->smoothImg;	
	ENSURE(currViewShape == nextViewImg.Shape());
	int nextViewSegID = this->nextView->seg->labelImg.Pixel(nextX, nextY, 0);
	const Segment &nextViewSeg = this->nextView->seg->segments[nextViewSegID];

	CameraMats currCamMats = this->currView->cam->cameraMats; 

	CHECK_RANGE(currViewSeg.segData->iDepth, 0, this->depthCount - 1);
	CHECK_RANGE(nextViewSeg.segData->iDepth, 0, this->depthCount - 1);
	double currViewSegDepth = this->depthPlanes[currViewSeg.segData->iDepth];
	double nextViewSegDepth = this->depthPlanes[nextViewSeg.segData->iDepth];

	float noiseVar = this->flowParams.noiseVar;

	flowCost *= getPixelMatchCost(currX, currY, currViewImg,
								  nextX, nextY, nextViewImg,
								  noiseVar);	

	for(uint iViewNeigh = 0; iViewNeigh < this->currViewNeighs.size(); iViewNeigh++)
	{
		CFloatImage currNeighViewImg = this->currViewNeighs[iViewNeigh].smoothImg;
		CShape currNeighViewShape = currNeighViewImg.Shape();

		CFloatImage nextNeighViewImg = this->nextViewNeighs[iViewNeigh].smoothImg;
		ENSURE(currNeighViewShape == nextNeighViewImg.Shape());

		CameraMats neighCamMats = this->currViewNeighs[iViewNeigh].cam->cameraMats;

		CVector3 currUVZ((double) currX, (double) currY, currViewSegDepth);
		CVector3 nextUVZ((double) nextX, (double) nextY, nextViewSegDepth);

		CVector3 currXYZ;
		CVector3 nextXYZ;

		CVector2 currNeighUV;
		CVector2 nextNeighUV;

		currCamMats.projUVZtoXYZ(currUVZ, currXYZ);
		neighCamMats.projXYZtoUV(currXYZ, currNeighUV);

		currCamMats.projUVZtoXYZ(nextUVZ, nextXYZ);
		neighCamMats.projXYZtoUV(nextXYZ, nextNeighUV);

		float neighCost = 0.0f;
		float currNeighOccAlpha = currViewSeg.segData->notOccProb[iViewNeigh];
		float nextNeighOccAlpha = nextViewSeg.segData->notOccProb[iViewNeigh];
		float occAlpha = currNeighOccAlpha * nextNeighOccAlpha;
		occAlpha = pow(occAlpha, 0.5f);

		if(currNeighViewShape.InBounds(currNeighUV[0], currNeighUV[1]))
		{
			if(currNeighViewShape.InBounds(nextNeighUV[0], nextNeighUV[1]))
			{
				int currNeighX = NEAREST_INT(currNeighUV[0]);
				int currNeighY = NEAREST_INT(currNeighUV[1]);
				int nextNeighX = NEAREST_INT(nextNeighUV[0]);
				int nextNeighY = NEAREST_INT(nextNeighUV[1]);

				neighCost = getPixelMatchCost(currNeighX, currNeighY, currNeighViewImg,
											  nextNeighX, nextNeighY, nextNeighViewImg, 
											  noiseVar);
			}
			else
			{
				neighCost = FLOW_OffScreen_Flowcost;
			}
		}
		else
		{
			neighCost = FLOW_OffScreen_Flowcost;
		}

		float weightedNeighCost = (occAlpha * neighCost);
		weightedNeighCost += (1.0f - occAlpha) * FLOW_Occ_Flowcost;
		weightedNeighCost /= 2.0f;
		SET_IF_GT(weightedNeighCost, 0.00001f);

		flowCost *= weightedNeighCost;		
	}	

	return flowCost;
}


float Flow::getPixelMatchCost(int x, int y, const CFloatImage &srcImg,
							  int xx, int yy, const CFloatImage &targetImg,
							  float noiseVar)
{
	float matchCost = 0.0f;

	float noiseVarDenom = -2.0f * noiseVar;
	CShape imgShape = srcImg.Shape();

	uint srcPixelAddr    = imgShape.PixelIndex(x, y, 0);
	uint targetPixelAddr = imgShape.PixelIndex(xx, yy, 0);
	for(int iBand = 0; iBand < imgShape.nBands; iBand++, srcPixelAddr++, targetPixelAddr++)
	{
		float diff = srcImg[srcPixelAddr] - targetImg[targetPixelAddr];
		float channelCost = exp((diff * diff) / noiseVarDenom);
		matchCost += channelCost;
	}
	matchCost /= imgShape.nBands;

	return matchCost;
}
