#include "Segmentation.h"
#include "Disk.h"
#include "ImageIO.h"

Segmentation::Segmentation()
{
	this->viewCount = 0;
	this->planeCount = 0;
	this->scenePlaneCount = 0;
	this->fullyComputed = false;
}

void Segmentation::CreateSegments()
{
	CShape maskShape = this->labelImg.Shape();
	int maxLabel = INT_MIN;
	int nodeCount = maskShape.width * maskShape.height;
	ENSURE(nodeCount > 0);
	for(int iNode = 0; iNode < nodeCount; iNode++)
	{
		int nodeLabel = this->labelImg[iNode];
		SET_IF_GT(maxLabel, nodeLabel);
	}
	ENSURE(maxLabel != INT_MIN);

	CreateSegments(maxLabel + 1);
}

void Segmentation::CreateSegments(int segCount)
{	
	this->segments.resize(segCount);
	Bounds defBounds(SHRT_MAX, SHRT_MAX, SHRT_MIN, SHRT_MIN);
	for(int iSeg = 0; iSeg  < segCount; iSeg++)
	{
		this->segments[iSeg].id = iSeg;
		ENSURE(this->segments[iSeg].bounds == defBounds);
		ENSURE(this->segments[iSeg].size == 0);
	}

	CShape imgShape = this->labelImg.Shape();
	for(short y = 0; y < imgShape.height; y++)
	for(short x = 0; x < imgShape.width;  x++)
	{
		int currPixelLabel = this->labelImg.Pixel(x, y, 0);
		CHECK_RANGE(currPixelLabel, 0, segCount - 1);

		Segment &currSeg = this->segments[currPixelLabel];
		currSeg.size += 1;		
		SET_IF_LT(currSeg.bounds.minX, x);
		SET_IF_LT(currSeg.bounds.minY, y);
		SET_IF_GT(currSeg.bounds.maxX, x);
		SET_IF_GT(currSeg.bounds.maxY, y);
	}

	ComputeSegAvgColors();
	CreateSegmentPoints();
}


void Segmentation::CreateSegmentPoints()
{
	INSIST(this->segments.size() > 0);

	Vector2<float> *defSegPoint          = new Vector2<float>[this->segments.size()];
	float          *dspDistSqrFromCenter = new float[this->segments.size()];
	Vector2<float> *segCenter            = new Vector2<float>[this->segments.size()];
	
	for(uint iSeg = 0; iSeg  < this->segments.size(); iSeg++)
	{
		Segment &currSeg = this->segments[iSeg];
		
		currSeg.point = Vector2<float>(0.0f, 0.0f);

		defSegPoint[iSeg]          = Vector2<float>(-1.0f, -1.0f);
		dspDistSqrFromCenter[iSeg] = FLT_MAX;
		segCenter[iSeg][0]         = currSeg.bounds.minX + ((currSeg.bounds.maxX - currSeg.bounds.minX) / 2.0f);
		segCenter[iSeg][1]         = currSeg.bounds.minY + ((currSeg.bounds.maxY - currSeg.bounds.minY) / 2.0f);
		
		INSIST(segCenter[iSeg][0] >= 0.0f);
		INSIST(segCenter[iSeg][1] >= 0.0f);
		INSIST(currSeg.size > 0);
	}

	CShape imgShape = this->labelImg.Shape();
	for(short y = 0; y < imgShape.height; y++)
	for(short x = 0; x < imgShape.width;  x++)
	{
		int currPixelLabel = this->labelImg.Pixel(x, y, 0);
		Segment &currSeg = this->segments[currPixelLabel];
		currSeg.point[0] += x;
		currSeg.point[1] += y;

		float distCenterX = x - segCenter[currPixelLabel][0];
		float distCenterY = y - segCenter[currPixelLabel][1];
		float distSqr = (distCenterX * distCenterX) + (distCenterY * distCenterY);
		if(distSqr < dspDistSqrFromCenter[currPixelLabel])
		{
			defSegPoint[currPixelLabel] = Vector2<float>(x,y);	
			dspDistSqrFromCenter[currPixelLabel] = distSqr;
		}		
	}

	for(uint iSeg = 0; iSeg  < this->segments.size(); iSeg++)
	{
		Segment &currSeg = this->segments[iSeg];
		currSeg.point = currSeg.point / (float) currSeg.size;

		int segX = NEAREST_INT(currSeg.point[0]);
		int segY = NEAREST_INT(currSeg.point[1]);
		int segPixelLabel = this->labelImg.Pixel(segX, segY, 0);
		if(segPixelLabel != currSeg.id)
		{	
			currSeg.point = defSegPoint[currSeg.id];
		}
	}

	delete [] defSegPoint;
	delete [] dspDistSqrFromCenter;
	delete [] segCenter;
}


void Segmentation::ComputeSegAvgColors()
{
	CShape imgShape = this->smoothImg.Shape();
	ENSURE(imgShape.nBands <= 3);
	ENSURE(imgShape.SameIgnoringNBands(this->labelImg.Shape()));
	ENSURE(this->labelImg.Shape().nBands == 1);

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		currSeg->avgCol = Vector3<float>(0.0f, 0.0f, 0.0f);
		ENSURE(currSeg->size > 0);
	}

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++)
	{
		int currPixelLabel = this->labelImg[nodeAddr];
		Segment &currSeg   = this->segments[currPixelLabel];

		for(int channel = 0; channel < imgShape.nBands; channel++, pixelAddr++)
		{
			//multiply and div by 255 to simulate integer disk write/read
			currSeg.avgCol[channel] += ((int) (this->smoothImg[pixelAddr] * 255.0f)) / 255.0f;
		}
	}

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		for(int channel = 0; channel < imgShape.nBands; channel++)
		{
			currSeg->avgCol[channel] /= currSeg->size;
		}
	}
}

void Segmentation::ComputeSpatialNeighs()
{
	ENSURE(this->viewCount > 0);
	ENSURE(this->planeCount > 0);

	CShape imgShape = this->smoothImg.Shape();

	const int connectivity = 2;
	const int dx[connectivity] = {-1,  0};
	const int dy[connectivity] = { 0, -1};

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	for(int x = 0; x < imgShape.width;  x++, nodeAddr++, pixelAddr+= imgShape.nBands)
	{
		int currNodeLabel = this->labelImg[nodeAddr];
		for(int iNeighbour = 0; iNeighbour < connectivity; iNeighbour++)
		{
			int neighbourX = x + dx[iNeighbour];
			int neighbourY = y + dy[iNeighbour];
			if(imgShape.InBounds(neighbourX, neighbourY) == false) continue;

			int neighNodeLabel = this->labelImg.Pixel(neighbourX, neighbourY, 0);
			if(neighNodeLabel == currNodeLabel) continue;
						
			Segment &currSeg  = this->segments[currNodeLabel];
			Segment &neighSeg = this->segments[neighNodeLabel];

			Vector3<float> colDiff(0.0f, 0.0f, 0.0f);
			uint neighPixelAddr = imgShape.PixelIndex(neighbourX, neighbourY, 0);
			for(int channel = 0; channel < imgShape.nBands; channel++)
			{
				colDiff[channel] = this->smoothImg[pixelAddr + channel] -
					               this->smoothImg[neighPixelAddr + channel];
			}
			//check - use actually boundary pixels instead of avg col?
			//Vector3<float> colDiff = currSeg.avgCol - neighSeg.avgCol;

			//check - weight each col channel differently?
			float colDist = colDiff.Length();

			currSeg.spatialNeighs.FoundNeighEdgel(neighNodeLabel, colDist);
			neighSeg.spatialNeighs.FoundNeighEdgel(currNodeLabel, colDist);
		}
	}

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		currSeg->spatialNeighs.DoneFindingNeighEdges();
	}
}

void Segmentation::AllocSegmentBuffers(int viewCount, int planeCount, int scenePlaneCount)
{
	this->viewCount       = viewCount;
	this->planeCount      = planeCount;
	this->scenePlaneCount = scenePlaneCount;
	ENSURE(this->viewCount > 0);
	ENSURE(this->planeCount > 0);
	ENSURE(this->scenePlaneCount >= 0);

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		currSeg->AllocBuffers(this->viewCount, this->planeCount, this->scenePlaneCount);
	}
}
