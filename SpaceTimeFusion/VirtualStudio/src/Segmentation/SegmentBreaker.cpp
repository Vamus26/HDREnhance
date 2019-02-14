#include "SegmentBreaker.h"
#include "ImageIO.h"
#include <algorithm>


SegmentBreaker::SegmentBreaker()
{
	this->brokenSegs = NULL;
	this->mergedSegs = NULL;
	this->gridCols = 0;
	this->gridRows = 0;
}

SegmentBreaker::~SegmentBreaker()
{
	if(this->brokenSegs != NULL)
	{
		ENSURE(this->mergedSegs != NULL);
		for(int iRow = 0; iRow < this->gridRows; iRow++)
		{
			delete [] this->brokenSegs[iRow];
			delete [] this->mergedSegs[iRow];
		}
		delete this->brokenSegs;
		delete this->mergedSegs;
	}
}

void SegmentBreaker::BreakSegment(int iSeg, Segmentation &segmentation, SegmentBreakerParams sbParams)
{
	this->params = sbParams;
	ENSURE(this->params.maxSegSize < USHRT_MAX);

	int gridArea = (this->params.breakGridSize * this->params.breakGridSize);
	ENSURE(this->params.breakGridSize > 1);
	ENSURE(this->params.maxSegSize >= 3 * gridArea);
	ENSURE(this->params.minSegSize <= gridArea);	
	ENSURE(iSeg < (int) segmentation.segments.size());
		
	Segment segToBreak = segmentation.segments[iSeg];
	ENSURE(segToBreak.size > this->params.minSegSize);

	init(segToBreak);

	createBrokenSegs(segToBreak, segmentation);	

	mergeSmallSegs();

	modifySegmentation(iSeg, segmentation);
}

void SegmentBreaker::init(const Segment &segToBreak)
{
	int segWidth  = (segToBreak.bounds.maxX - segToBreak.bounds.minX) + 1;
	int segHeight = (segToBreak.bounds.maxY - segToBreak.bounds.minY) + 1;
	ENSURE((segWidth > 0) && (segHeight > 0));

	this->gridCols = ((segWidth  - 1) / this->params.breakGridSize) + 1;
	this->gridRows = ((segHeight - 1) / this->params.breakGridSize) + 1;

	Bounds defBounds(SHRT_MAX, SHRT_MAX, SHRT_MIN, SHRT_MIN);

	this->brokenSegs = new Segment*[this->gridRows];
	this->mergedSegs = new Segment*[this->gridRows];
	ENSURE(this->brokenSegs != NULL);
	ENSURE(this->mergedSegs != NULL);
	for(int iRow = 0; iRow < this->gridRows; iRow++)
	{
		this->brokenSegs[iRow] = new Segment[this->gridCols];
		this->mergedSegs[iRow] = new Segment[this->gridCols];
		ENSURE(this->brokenSegs[iRow] != NULL);
		ENSURE(this->mergedSegs[iRow] != NULL);
		for(int iCol = 0; iCol < this->gridCols; iCol++)
		{
			Segment &brokenSeg = this->brokenSegs[iRow][iCol];
			brokenSeg.id = getGridID(iRow, iCol);
			brokenSeg.size = 0;
			brokenSeg.bounds = defBounds;
		}
	}
}

void SegmentBreaker::createBrokenSegs(const Segment &segToBreak, const Segmentation &segmentation)
{
	Bounds segBounds = segToBreak.bounds;
	const CIntImage &labelImg = segmentation.labelImg;
	int segPixelsFound = 0;
	for(short y = segBounds.minY; y <= segBounds.maxY; y++)
	{
		int gridY = (y - segBounds.minY) / this->params.breakGridSize;

		for(short x = segBounds.minX; x <= segBounds.maxX; x++)
		{
			int gridX = (x - segBounds.minX) / this->params.breakGridSize;

			if(labelImg.Pixel(x, y, 0) == segToBreak.id)
			{
				Segment &brokenSeg = this->brokenSegs[gridY][gridX];
				brokenSeg.size += 1;
				SET_IF_LT(brokenSeg.bounds.minX, x);
				SET_IF_LT(brokenSeg.bounds.minY, y);
				SET_IF_GT(brokenSeg.bounds.maxX, x);
				SET_IF_GT(brokenSeg.bounds.maxY, y);
				segPixelsFound++;
			}
		}
	}
	ENSURE(segPixelsFound == segToBreak.size);

	for(int iRow = 0; iRow < this->gridRows; iRow++)
	for(int iCol = 0; iCol < this->gridCols; iCol++)
	{
		this->mergedSegs[iRow][iCol] = this->brokenSegs[iRow][iCol];			
	}
}

void SegmentBreaker::mergeSmallSegs()
{
	vector<bool> ignoreList(this->gridCols * this->gridRows, false);

	const int connectivity = 8;
	const int dx[connectivity] = {  0, -1,  0,  1, -1, -1,  1, 1};
	const int dy[connectivity] = { -1,  0,  1,  0, -1,  1, -1, 1};

	while(true)
	{
		int brokenSegGridID = getMinMergedSegGridID(ignoreList);
		if(brokenSegGridID == -1)
		{
			break;
		}
		int brokenSegRow, brokenSegCol;
		getGridRowAndCol(brokenSegGridID, &brokenSegRow, &brokenSegCol);

		Segment &brokenSeg = this->brokenSegs[brokenSegRow][brokenSegCol];
		Segment &mergedSeg = this->mergedSegs[brokenSegRow][brokenSegCol];
		ENSURE(mergedSeg.size > 0);
		if(mergedSeg.size >= this->params.minSegSize)
		{
			break;
		}

		int minNeighGridID = -1;
		Segment minNeighMergedSeg;
		minNeighMergedSeg.size = USHRT_MAX;

		for(int iNeighbour = 0; iNeighbour < connectivity; iNeighbour++)
		{
			if(iNeighbour == 4)
			{
				if(minNeighGridID != -1) 
					break;
			}

			int neighbourCol = brokenSegCol + dx[iNeighbour];
			int neighbourRow = brokenSegRow + dy[iNeighbour];
			if((neighbourCol < 0) || 
			   (neighbourRow < 0) ||
			   (neighbourCol >= this->gridCols) ||
			   (neighbourRow >= this->gridRows))
			{
				continue;
			}

			Segment &neighBrokenSeg = this->brokenSegs[neighbourRow][neighbourCol];
			Segment &neighMergedSeg = this->mergedSegs[neighbourRow][neighbourCol];

			if(neighMergedSeg.size == 0) continue;			
			if(neighMergedSeg.id == mergedSeg.id) continue;			

			if(neighMergedSeg.size < minNeighMergedSeg.size)
			{
				bool xConnectivity = true;
				bool yConnectivity = true;

				Bounds currGridBounds  = brokenSeg.bounds;
				Bounds neighGridBounds = neighBrokenSeg.bounds;

				//bool ySpanOverlap = ( (iNeighbour >= 4) || (dx[iNeighbour] == 0) ||
				//	                  ((neighGridBounds.maxY >= currGridBounds.minY) && 
				//					   (neighGridBounds.minY <= currGridBounds.maxY)) );

				//bool xSpanOverlap = ( (iNeighbour >= 4) || (dy[iNeighbour] == 0) ||
				//					  ((neighGridBounds.maxX >= currGridBounds.minX) && 
				//					   (neighGridBounds.minX <= currGridBounds.maxX)) );

				bool ySpanOverlap = ( (dx[iNeighbour] == 0) ||
					                  ((neighGridBounds.maxY >= currGridBounds.minY - 1) && 
									   (neighGridBounds.minY <= currGridBounds.maxY + 1)) );

				bool xSpanOverlap = ( (dy[iNeighbour] == 0) ||
									  ((neighGridBounds.maxX >= currGridBounds.minX - 1) && 
									   (neighGridBounds.minX <= currGridBounds.maxX + 1)) );


				if(dx[iNeighbour] < 0)
					xConnectivity = ySpanOverlap && (neighGridBounds.maxX + 1 == currGridBounds.minX);
				else if(dx[iNeighbour] > 0)
					xConnectivity = ySpanOverlap && (currGridBounds.maxX + 1 == neighGridBounds.minX);
	
				if(dy[iNeighbour] < 0)
					yConnectivity = xSpanOverlap && (neighGridBounds.maxY + 1 == currGridBounds.minY);
				else if(dy[iNeighbour] > 0)
					yConnectivity = xSpanOverlap && (currGridBounds.maxY + 1 == neighGridBounds.minY);

				if(xConnectivity && yConnectivity)
				{
					minNeighMergedSeg = neighMergedSeg;
					minNeighGridID   = getGridID(neighbourRow, neighbourCol);
					ENSURE(ignoreList[minNeighGridID] == false);
				}
			}			
		}

		if(minNeighGridID == -1)
		{			
			ignoreList[brokenSegGridID] = true;
			continue;		
		}
		else
		{
			ENSURE(minNeighMergedSeg.size >= mergedSeg.size);
		}

		int neighbourRow, neighbourCol;
		getGridRowAndCol(minNeighGridID, &neighbourRow, &neighbourCol);

		Segment newMergedSeg;
		newMergedSeg.id = minNeighMergedSeg.id;
		newMergedSeg.size = minNeighMergedSeg.size + mergedSeg.size;
		newMergedSeg.bounds = getMergedBounds(minNeighMergedSeg.bounds, mergedSeg.bounds);
		updateMergedSegs(mergedSeg.id, newMergedSeg);
	}	
}

void SegmentBreaker::updateMergedSegs(int oldMergedSegID, Segment &newMergedSeg)
{
	for(int iRow = 0; iRow < this->gridRows; iRow++)
	for(int iCol = 0; iCol < this->gridCols; iCol++)
	{
		Segment &mergedSeg = this->mergedSegs[iRow][iCol];
		if((mergedSeg.id == oldMergedSegID) ||
		   (mergedSeg.id == newMergedSeg.id))
		{
			mergedSeg = newMergedSeg;
		}
	}
}

int SegmentBreaker::getMinMergedSegGridID(vector<bool> &ignoreList)
{
	int minMergedSegGridID = -1;
	int minMergedSegSize   = INT_MAX;

	int gridID = 0;
	for(int iRow = 0; iRow < this->gridRows; iRow++)
	for(int iCol = 0; iCol < this->gridCols; iCol++, gridID++)
	{
		if(ignoreList[gridID] == false)
		{
			Segment &mergedSeg = this->mergedSegs[iRow][iCol];
			if((mergedSeg.size > 0) &&
			   (mergedSeg.size < minMergedSegSize))
			{
				minMergedSegSize = mergedSeg.size;
				minMergedSegGridID = gridID;
			}
		}
	}

	return minMergedSegGridID;
}

void SegmentBreaker::modifySegmentation(int iSeg, Segmentation &segmentation)
{	
	vector<Segment> &segments = segmentation.segments;

	int segsFound = generateNewSegIDs(segments[iSeg], segmentation);	
	if(segsFound == 1)
	{
		return;
	}

	Bounds segBounds = segments[iSeg].bounds;
	Bounds defBounds(SHRT_MAX, SHRT_MAX, SHRT_MIN, SHRT_MIN);
	
	segments[iSeg].size = 0;
	int segID_offset = (int) segmentation.segments.size();
	segmentation.segments.reserve(segID_offset + segsFound - 1);
	for(int iNewSeg = 0; iNewSeg < (segsFound - 1); iNewSeg++)
	{
		Segment newSeg;
		newSeg.id = iNewSeg + segID_offset;
		newSeg.size = 0;
		segmentation.segments.push_back(newSeg);
	}

	for(int iRow = 0; iRow < this->gridRows; iRow++)
	for(int iCol = 0; iCol < this->gridCols; iCol++)
	{
		Segment &mergedSeg = this->mergedSegs[iRow][iCol];

		if(mergedSeg.size == 0)	continue;

		Segment &currSeg = segmentation.segments[mergedSeg.id];
		if(currSeg.size == 0)
		{
			currSeg = mergedSeg;
		}
		else
		{
			ENSURE(currSeg.size == mergedSeg.size);
		}
	}

	CIntImage &labelImg = segmentation.labelImg;
	for(int y = segBounds.minY; y <= segBounds.maxY; y++)
	{
		int gridY = (y - segBounds.minY) / this->params.breakGridSize;

		for(int x = segBounds.minX; x <= segBounds.maxX; x++)
		{
			int gridX = (x - segBounds.minX) / this->params.breakGridSize;

			if(labelImg.Pixel(x, y, 0) == segments[iSeg].id)
			{
				Segment &mergedSeg = this->mergedSegs[gridY][gridX];
				ENSURE(mergedSeg.size > 0);
				labelImg.Pixel(x, y, 0) = mergedSeg.id;
			}
		}
	}
}	


int SegmentBreaker::generateNewSegIDs(const Segment &segToBreak, const Segmentation &segmentation)
{
	vector<int> newSegIDs(this->gridCols * this->gridRows, 0);

	int segsFound = 0;
	for(int i = 0; i <= 1; i++)
	for(int iRow = 0; iRow < this->gridRows; iRow++)
	for(int iCol = 0; iCol < this->gridCols; iCol++)
	{
		Segment &mergedSeg = this->mergedSegs[iRow][iCol];

		if(mergedSeg.size > 0)
		{
			int gridID = getGridID(iRow, iCol);
			int parentGridID = mergedSeg.id;

			if(i == 0)
			{
				if(parentGridID == gridID)
				{
					int segID;
					if(segsFound == 0)
						segID = segToBreak.id;
					else
						segID = (int) segmentation.segments.size() + (segsFound - 1);
	
					newSegIDs[gridID] = segID;
					segsFound++;
				}
			}
			else
			{
				mergedSeg.id = newSegIDs[parentGridID];							
			}
		}
	}
	ENSURE(segsFound > 0);

	return segsFound;
}


Bounds SegmentBreaker::getMergedBounds(const Bounds &bounds1, const Bounds &bounds2)
{
	Bounds mergedBounds;

	ENSURE(bounds1.minX <= bounds1.maxX);
	ENSURE(bounds1.minY <= bounds1.maxY);
	ENSURE(bounds2.minX <= bounds2.maxX);	
	ENSURE(bounds2.minY <= bounds2.maxY);
	
	mergedBounds.minX = std::min(bounds1.minX, bounds2.minX);
	mergedBounds.minY = std::min(bounds1.minY, bounds2.minY);
	mergedBounds.maxX = std::max(bounds1.maxX, bounds2.maxX);
	mergedBounds.maxY = std::max(bounds1.maxY, bounds2.maxY);	

	return mergedBounds;
}
