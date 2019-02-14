#include "Flow.h"
#include "ImageProcessing.h"
#include "ImageIO.h"
#include "Draw.h"

void Flow::getCurrViewFlowVis(CFloatImage &uVisImg, CFloatImage &vVisImg, float maxFlow)
{
	ENSURE(maxFlow > 0.0f);
	ENSURE(this->currView != NULL);
	vector<Segment> &currViewSegs = this->currView->seg->segments;
	const CIntImage &labelImg = this->currView->seg->labelImg;
	CShape maskShape = labelImg.Shape();

	uVisImg.ReAllocate(maskShape);
	vVisImg.ReAllocate(maskShape);
	uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	{
		for(int x = 0; x < maskShape.width; x++, nodeAddr++)
		{
			int nodeLabel = labelImg[nodeAddr];
			Segment &nodeSeg = currViewSegs[nodeLabel];
			SegFlowData *segFlowData = (SegFlowData *) nodeSeg.auxData;

			ENSURE(segFlowData != NULL);

			uVisImg[nodeAddr] = (float) segFlowData->xOffset;
			vVisImg[nodeAddr] = (float) segFlowData->yOffset;
			ENSURE(fabs(uVisImg[nodeAddr]) <= maxFlow);
			ENSURE(fabs(vVisImg[nodeAddr]) <= maxFlow);
		}
	}

	ImageProcessing::Operate(uVisImg, uVisImg, maxFlow, ImageProcessing::OP_ADD);
	ImageProcessing::Operate(vVisImg, vVisImg, maxFlow, ImageProcessing::OP_ADD);
	ImageProcessing::Rescale(uVisImg, 0.0f, 1.0f);
	ImageProcessing::Rescale(vVisImg, 0.0f, 1.0f);
}


void Flow::visFlowProjections(int currX, int currY, int offsetX, int offsetY, bool flipY)
{	
	CFloatImage currViewImg = this->currView->smoothImg;
	CShape currViewShape = currViewImg.Shape();
	ENSURE(currViewShape.InBounds(currX, currY));

	if(flipY)
		currY = currViewShape.height - currY - 1;

	int nextX = currX + offsetX;
	int nextY = currY + offsetY;
	ENSURE(currViewShape.InBounds(nextX, nextY));

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

		if(currNeighViewShape.InBounds(currNeighUV[0], currNeighUV[1]))
		{
			if(currNeighViewShape.InBounds(nextNeighUV[0], nextNeighUV[1]))
			{
				int currNeighX = NEAREST_INT(currNeighUV[0]);
				int currNeighY = NEAREST_INT(currNeighUV[1]);
				int nextNeighX = NEAREST_INT(nextNeighUV[0]);
				int nextNeighY = NEAREST_INT(nextNeighUV[1]);

				char filename[1024];
				CVector3 col(1.0, 0.0, 0.0);
				CFloatImage currNeigh = currNeighViewImg.Clone();
				Draw::DrawPoint(currNeigh, currNeighX, currNeighY, 2, col);
				CFloatImage nextNeigh = nextNeighViewImg.Clone();
				Draw::DrawPoint(nextNeigh, nextNeighX, nextNeighY, 2, col);
				sprintf(filename, "currNeigh-%i.tga", iViewNeigh);
				ImageIO::WriteFile(currNeigh, filename);
				sprintf(filename, "nextNeigh-%i.tga", iViewNeigh);
				ImageIO::WriteFile(nextNeigh, filename);
			}
			else
			{
				REPORT("Projected point out of bounds in nextNeigh-%i\n", iViewNeigh);
			}
		}
		else
		{
			REPORT("Projected point out of bounds in currNeigh-%i\n", iViewNeigh);
		}
	}	

	CVector3 col(1.0, 0.0, 0.0);
	CFloatImage curr = currViewImg.Clone();
	Draw::DrawPoint(curr, currX, currY, 2, col);
	CFloatImage next = nextViewImg.Clone();
	Draw::DrawPoint(next, nextX, nextY, 2, col);
	ImageIO::WriteFile(curr, "curr.tga");
	ImageIO::WriteFile(next, "next.tga");
}
