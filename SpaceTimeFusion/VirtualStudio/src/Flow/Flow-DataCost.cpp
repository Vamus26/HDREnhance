#include "Flow.h"
#include "ImageProcessing.h"
#include "ImageIO.h"
#include "Disk.h"

CFloatImage Flow::getCurrViewDataCostImg(int offsetX, int offsetY)
{	
	CFloatImage dataCostImg;
	Camera *currViewCam = this->currView->cam;
	int iCurrViewFrame = this->currView->iFrame;
	string flowDC_fn = currViewCam->GetFrameFlowDataCostFN(iCurrViewFrame, offsetX, offsetY);

	if(Disk::DoesFileExist(flowDC_fn))
	{
		ImageIO::ReadRaw(dataCostImg, flowDC_fn);
		ENSURE(dataCostImg.Shape() == this->currView->seg->labelImg.Shape());
	}
	else
	{
		dataCostImg = computeCurrViewDataCostImg(offsetX, offsetY);
		ImageIO::WriteRaw(dataCostImg, flowDC_fn);	
	}

	return dataCostImg;
}


CFloatImage Flow::computeCurrViewDataCostImg(int offsetX, int offsetY)
{
	CHECK_RANGE(offsetX, -this->flowParams.searchWinSize, this->flowParams.searchWinSize);
	CHECK_RANGE(offsetY, -this->flowParams.searchWinSize, this->flowParams.searchWinSize);

	const CIntImage &currLabelImg = this->currView->seg->labelImg;
	const CIntImage &nextLabelImg = this->nextView->seg->labelImg;
	CShape maskShape = currLabelImg.Shape();
	ENSURE(maskShape == nextLabelImg.Shape());

	const CFloatImage &currViewImg = this->currView->smoothImg;
	const CFloatImage &nextViewImg = this->nextView->smoothImg;

	const vector<Segment> &currViewSegs = this->currView->seg->segments;
	const vector<Segment> &nextViewSegs = this->nextView->seg->segments;

	const CameraMats &currCamMats = this->currView->cam->cameraMats;

	CFloatImage dataCostImg(maskShape);			

	uint nodeAddr = 0;
	for(float currY = 0; currY < maskShape.height; currY++)
	{
		for(float currX = 0; currX < maskShape.width; currX++, nodeAddr++)
		{
			float dataCost = 0.0f;
			float nextX = currX + offsetX;
			float nextY = currY + offsetY;

			if(maskShape.InBounds(nextX, nextY))
			{
				int currViewSegID = currLabelImg[nodeAddr];
				int nextViewSegID = nextLabelImg[nodeAddr];

				const Segment &currViewSeg = currViewSegs[currViewSegID];
				const Segment &nextViewSeg = nextViewSegs[nextViewSegID];

				double currViewSegDepth = this->depthPlanes[currViewSeg.segData->iDepth];
				double nextViewSegDepth = this->depthPlanes[nextViewSeg.segData->iDepth];

				CVector3 currUVZ(currX, currY, currViewSegDepth);
				CVector3 nextUVZ(nextX, nextY, nextViewSegDepth);

				float neighViewsDataCost;
				float neighViewsDataCostWeight;
				getNeighViewsDataCost(currUVZ, nextUVZ, currCamMats,
									  currViewSeg, nextViewSeg,
									  &neighViewsDataCost, &neighViewsDataCostWeight);

				dataCost = getPixelDataCost(currX, currY, currViewImg, 
											nextX, nextY, nextViewImg);
				dataCost += neighViewsDataCost;
				dataCost /= (1.0f + neighViewsDataCostWeight);
			}
			else
			{
				dataCost = FLOW_OffScreen_DataCost;
			}

			dataCostImg[nodeAddr] = dataCost;
		}
	}

	return dataCostImg;
}

void Flow::getNeighViewsDataCost(const CVector3 &currUVZ, const CVector3 &nextUVZ, const CameraMats &currCamMats,
								 const Segment &currViewSeg, const Segment &nextViewSeg,
								 float *neighViewsDataCost, float *neighViewsDataCostWeight)
{
	*neighViewsDataCost = 0.0f;
	*neighViewsDataCostWeight = 0.0f;


	for(uint iViewNeigh = 0; iViewNeigh < this->currViewNeighs.size(); iViewNeigh++)
	{
		CFloatImage currNeighViewImg = this->currViewNeighs[iViewNeigh].smoothImg;
		CShape currNeighViewShape = currNeighViewImg.Shape();

		CFloatImage nextNeighViewImg = this->nextViewNeighs[iViewNeigh].smoothImg;
		ENSURE(currNeighViewShape == nextNeighViewImg.Shape());

		const CameraMats &neighCamMats = this->currViewNeighs[iViewNeigh].cam->cameraMats;

		CVector3 currXYZ;
		CVector2 currNeighUV;
		currCamMats.projUVZtoXYZ(currUVZ, currXYZ);
		neighCamMats.projXYZtoUV(currXYZ, currNeighUV);			
		
		if(currNeighViewShape.InBounds(currNeighUV[0], currNeighUV[1]))
		{
			CVector3 nextXYZ;
			CVector2 nextNeighUV;
			currCamMats.projUVZtoXYZ(nextUVZ, nextXYZ);
			neighCamMats.projXYZtoUV(nextXYZ, nextNeighUV);

			if(currNeighViewShape.InBounds(nextNeighUV[0], nextNeighUV[1]))
			{
				float currNeighOccAlpha = currViewSeg.segData->notOccProb[iViewNeigh];
				float nextNeighOccAlpha = nextViewSeg.segData->notOccProb[iViewNeigh];
				float occAlpha = currNeighOccAlpha * nextNeighOccAlpha;
				occAlpha = pow(occAlpha, 0.5f);

				float neighDataCost = getPixelDataCost((float) currNeighUV[0], (float) currNeighUV[1], currNeighViewImg,
												       (float) nextNeighUV[0], (float) nextNeighUV[1], nextNeighViewImg);

				*neighViewsDataCost += neighDataCost * occAlpha;
				*neighViewsDataCostWeight += occAlpha;
			}
		}		
	}
}

float Flow::getPixelDataCost(int currX, int currY, int offsetX, int offsetY)
{
	float dataCost = 0.0f;
	float dataCostWeight = 0.0f;

	int nextX = currX + offsetX;
	int nextY = currY + offsetY;

	CFloatImage currViewImg = this->currView->smoothImg;
	CShape currViewShape = currViewImg.Shape();
	if(currViewShape.InBounds(nextX, nextY) == false)
	{
		return FLOW_OffScreen_DataCost;
	}

	int currViewSegID = this->currView->seg->labelImg.Pixel(currX, currY, 0);
	Segment &currViewSeg = this->currView->seg->segments[currViewSegID];

	CFloatImage nextViewImg = this->nextView->smoothImg;	
	ENSURE(currViewShape == nextViewImg.Shape());
	int nextViewSegID = this->nextView->seg->labelImg.Pixel(nextX, nextY, 0);
	const Segment &nextViewSeg = this->nextView->seg->segments[nextViewSegID];

	CameraMats currCamMats = this->currView->cam->cameraMats; 

	CHECK_RANGE(currViewSeg.segData->iDepth, 0, this->depthCount - 1);
	CHECK_RANGE(nextViewSeg.segData->iDepth, 0, this->depthCount - 1);
	double currViewSegDepth = this->depthPlanes[currViewSeg.segData->iDepth];
	double nextViewSegDepth = this->depthPlanes[nextViewSeg.segData->iDepth];


	dataCost += getPixelDataCost((float) currX, (float) currY, currViewImg,
							     (float) nextX, (float) nextY, nextViewImg);
	dataCostWeight += 1.0f;

	for(uint iViewNeigh = 0; iViewNeigh < this->currViewNeighs.size(); iViewNeigh++)
	{
		CFloatImage currNeighViewImg = this->currViewNeighs[iViewNeigh].smoothImg;
		CShape currNeighViewShape = currNeighViewImg.Shape();

		CFloatImage nextNeighViewImg = this->nextViewNeighs[iViewNeigh].smoothImg;
		ENSURE(currNeighViewShape == nextNeighViewImg.Shape());

		CameraMats neighCamMats = this->currViewNeighs[iViewNeigh].cam->cameraMats;

		CVector3 currUVZ((double) currX, (double) currY, currViewSegDepth);
		CVector3 currXYZ;
		CVector2 currNeighUV;
		currCamMats.projUVZtoXYZ(currUVZ, currXYZ);
		neighCamMats.projXYZtoUV(currXYZ, currNeighUV);
			
		CVector3 nextUVZ((double) nextX, (double) nextY, nextViewSegDepth);
		CVector3 nextXYZ;
		CVector2 nextNeighUV;		
		currCamMats.projUVZtoXYZ(nextUVZ, nextXYZ);
		neighCamMats.projXYZtoUV(nextXYZ, nextNeighUV);		

		float currNeighOccAlpha = currViewSeg.segData->notOccProb[iViewNeigh];
		float nextNeighOccAlpha = nextViewSeg.segData->notOccProb[iViewNeigh];
		float occAlpha = currNeighOccAlpha * nextNeighOccAlpha;
		occAlpha = pow(occAlpha, 0.5f);

		if(currNeighViewShape.InBounds(currNeighUV[0], currNeighUV[1]))
		{
			if(currNeighViewShape.InBounds(nextNeighUV[0], nextNeighUV[1]))
			{
				float neighDataCost = getPixelDataCost((float) currNeighUV[0], (float) currNeighUV[1], currNeighViewImg,
												       (float) nextNeighUV[0], (float) nextNeighUV[1], nextNeighViewImg);

				dataCost += neighDataCost * occAlpha;
				dataCostWeight += occAlpha;
			}
		}		
	}	

	dataCost /= dataCostWeight;

	return dataCost;
}

float Flow::getPixelDataCostBirch(float srcX, float srcY, const CFloatImage &srcImg,
							      float dstX, float dstY, const CFloatImage &dstImg)
{
	float dataCost = FLT_MAX;

	CShape imgShape = srcImg.Shape();
	for(float yOffset = -0.5; yOffset <= 0.5; yOffset += 0.5)
	{
		for(float xOffset = -0.5; xOffset <= 0.5; xOffset += 0.5)
		{
			float dist = 0.0f;
			float dstXX = dstX + xOffset;
			float dstYY = dstY + yOffset;
			for(int channel = 0; channel < imgShape.nBands; channel++)
			{
				float srcChannelVal = ImageProcessing::BilinearIterpolation(srcImg, srcX, srcY, channel);
				float dstChannelVal = ImageProcessing::BilinearIterpolation(dstImg, dstXX, dstYY, channel);
				float channelDiff = (srcChannelVal - dstChannelVal);
				//dist += (channelDiff * channelDiff);
				dist += fabs(channelDiff);
			}
			//dist = sqrt(dist);
			dist /= imgShape.nBands;
			SET_IF_LT(dataCost, dist);
		}
	}

	SET_IF_LT(dataCost, this->flowParams.maxDataCost);

	return dataCost;
}

float Flow::getPixelDataCostFast(float srcX, float srcY, const CFloatImage &srcImg,
								 float dstX, float dstY, const CFloatImage &dstImg)
{
	float dataCost = 0.0f;

	CShape imgShape = srcImg.Shape();

	uint srcPixelAddr = imgShape.PixelIndex(NEAREST_INT(srcX), NEAREST_INT(srcY), 0);
	uint dstPixelAddr = imgShape.PixelIndex(NEAREST_INT(dstX), NEAREST_INT(dstY), 0);
	for(int channel = 0; channel < imgShape.nBands; channel++, srcPixelAddr++, dstPixelAddr++)
	{
		float channelDiff = srcImg[srcPixelAddr] - dstImg[dstPixelAddr];
		dataCost += fabs(channelDiff);
	}
	dataCost /= imgShape.nBands;

	SET_IF_LT(dataCost, this->flowParams.maxDataCost);

	return dataCost;
}

