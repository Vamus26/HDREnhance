#include "Segmentation.h"
#include "ImageIO.h"

void Segmentation::CheckSegmentationConsistency(int minSegSize, int maxSegSize)
{
	CShape imgShape = this->labelImg.Shape();

	ENSURE(this->segments.size() > 0);

	uint nodeAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++)
	{
		int currPixelLabel = this->labelImg[nodeAddr];
		const Segment &currSeg = this->segments[currPixelLabel];
		CHECK_RANGE(x, currSeg.bounds.minX, currSeg.bounds.maxX);
		CHECK_RANGE(y, currSeg.bounds.minY, currSeg.bounds.maxY);
	}

	for(uint iSeg = 0; iSeg < this->segments.size(); iSeg++)
	{
		const Segment &currSeg = this->segments[iSeg];
		ENSURE(currSeg.id == (int) iSeg);

		CHECK_RANGE(currSeg.size, 1, USHRT_MAX);

		int segX = NEAREST_INT(currSeg.point[0]);
		int segY = NEAREST_INT(currSeg.point[1]);
		int segPointLabel = this->labelImg.Pixel(segX, segY, 0);
		INSIST(segPointLabel == currSeg.id);

		//if(currSeg.size < minSegSize)
		//{
		//	printf("Warning. Small segment remains. Segment-%05i, size - %i\n", iSeg, currSeg.size);
		//}
		//if(segments[iSeg].size > maxSegSize)
		//{		
		//	printf("Warning. Large segment remains. Segment-%05i, size - %i\n", iSeg, currSeg.size);
		//}
		currSeg.CheckBoundTighness(this->labelImg);
		currSeg.CheckSizeAndColorConsistency(this->labelImg, this->smoothImg);		
	}
}

CByteImage Segmentation::GetBoundaryImg()
{
	CShape imgShape = this->labelImg.Shape();
	
	CByteImage boundaryImg(imgShape.width, imgShape.height, 3);
	boundaryImg.ClearPixels();

	const int connectivity = 2;
	const int dx[connectivity] = {-1,  0};
	const int dy[connectivity] = { 0, -1};

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++, pixelAddr += 3)
	{
		int currPixelLabel = this->labelImg[nodeAddr];
		for(int iNeighbour = 0; iNeighbour < connectivity; iNeighbour++)
		{
			int neighbourX = x + dx[iNeighbour];
			int neighbourY = y + dy[iNeighbour];

			if(imgShape.InBounds(neighbourX, neighbourY) == true)
			{
				if(this->labelImg.Pixel(neighbourX, neighbourY, 0) != currPixelLabel)
				{
					boundaryImg[pixelAddr + 0] = MASK_VALID;
					boundaryImg[pixelAddr + 1] = MASK_VALID;
					boundaryImg[pixelAddr + 2] = MASK_VALID;
					break;
				}
			}
		}
	}

	for(uint iSeg = 0; iSeg < this->segments.size(); iSeg++)
	{
		const Segment &currSeg = this->segments[iSeg];
		int segX = NEAREST_INT(currSeg.point[0]);
		int segY = NEAREST_INT(currSeg.point[1]);
		INSIST(imgShape.InBounds(segX, segY));
		boundaryImg.Pixel(segX, segY, 0) = MASK_VALID;		
	}

	return boundaryImg;
}

CFloatImage Segmentation::GetDatacostImg(int iPlane)
{
	ENSURE(this->segments.size() > 0);
	ENSURE(this->segments[0].segData != NULL);
	ENSURE(this->segments[0].segData->planeCount > iPlane);

	CShape maskShape = this->labelImg.Shape();	
	CFloatImage datacostImg(maskShape);
	uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width; x++, nodeAddr++)
	{
		int nodeLabel = this->labelImg[nodeAddr];
		Segment &currSeg = this->segments[nodeLabel];
		datacostImg[nodeAddr] = currSeg.segData->GetPlaneDatacost(iPlane);
	}

	return datacostImg;
}


CFloatImage Segmentation::GetSegmentationImg()
{
	ENSURE(this->segments.size() > 0);

	CShape imgShape = this->smoothImg.Shape();	
	CFloatImage segImg(imgShape);
	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++)
	{
		int nodeLabel    = this->labelImg[nodeAddr];
		Segment &currSeg = this->segments[nodeLabel];
		for(int channel = 0; channel < imgShape.nBands; channel++, pixelAddr++)
		{
			segImg[pixelAddr] = currSeg.avgCol[channel];
		}			
	}

	return segImg;
}


void Segmentation::DumpSegAndNeighVis(int segID)
{
	CHECK_RANGE(segID, 0, (int) this->segments.size() - 1);
	char filename[1024];

	CByteImage boundaryImg = GetBoundaryImg();
	ImageIO::WriteFile(boundaryImg, "boundary.tga");

	Segment &currSeg = this->segments[segID];
	sprintf(filename, "seg-%06i.tga", segID);
	CByteImage segVisImg = currSeg.GetSegVis(this->labelImg);
	ImageIO::WriteFile(segVisImg, filename);

	ENSURE(currSeg.spatialNeighs.neighCount > 0);
	for(int iNeigh = 0; iNeigh < currSeg.spatialNeighs.neighCount; iNeigh++)
	{
		int neighID = currSeg.spatialNeighs.neighID[iNeigh];

		Segment &neighSeg = this->segments[neighID];
		sprintf(filename, "neigh-%06i.tga", neighID);
		CByteImage neighVisImg = neighSeg.GetSegVis(this->labelImg);
		ImageIO::WriteFile(neighVisImg, filename);
	}
}

int Segmentation::GetSegID(int x, int y, bool flipY)
{
	CShape maskShape = this->labelImg.Shape();
	CHECK_RANGE(x, 0, maskShape.width - 1);
	CHECK_RANGE(y, 0, maskShape.height - 1);

	if(flipY)
		y = maskShape.height - y - 1;


	return this->labelImg.Pixel(x, y, 0);
}

CFloatImage Segmentation::GetNotOccProbImg(int iView)
{
	ENSURE(this->segments.size() > 0);
	CHECK_RANGE(iView, 0, this->viewCount - 1);

	CShape maskShape = this->labelImg.Shape();	
	CFloatImage notOccProbImg(maskShape);

	uint nodeAddr = 0;
	for(int y = 0; y < maskShape.height; y++)
	for(int x = 0; x < maskShape.width;  x++, nodeAddr++)
	{
		int nodeLabel = this->labelImg[nodeAddr];
		Segment &currSeg = this->segments[nodeLabel];
		notOccProbImg[nodeAddr] = currSeg.segData->notOccProb[iView];
	}

	return notOccProbImg;
}
