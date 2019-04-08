#ifndef __SEGMENT_BREAKER_H__
#define __SEGMENT_BREAKER_H__

#include "Definitions.h"
#include "Segment.h"
#include "Segmentation.h"

class SegmentBreaker
{

public:
	typedef struct _SegmentBreakerParams
	{
		int maxSegSize;
		int minSegSize;
		int breakGridSize;

		static _SegmentBreakerParams Default();
		void Dump(string prefix);

	} SegmentBreakerParams;

	SegmentBreakerParams params;

	Segment **brokenSegs;
	Segment **mergedSegs;
	int gridCols;
	int gridRows;

public: //SegmentBreaker.cpp
	SegmentBreaker();
	~SegmentBreaker();

	void BreakSegment(int iSeg, Segmentation &segmentation, SegmentBreakerParams sbParams);

private: //SegmentBreaker.cpp
	void init(const Segment &segToBreak);

	void createBrokenSegs(const Segment &segToBreak, const Segmentation &segmentation);

	void mergeSmallSegs();
	int getMinMergedSegGridID(vector<bool> &ignoreList); 
	void updateMergedSegs(int oldParentID, Segment &newParentSeg);
	Bounds getMergedBounds(const Bounds &bounds1, const Bounds &bounds2);

	void modifySegmentation(int iSeg, Segmentation &segmentation);
	int generateNewSegIDs(const Segment &segToBreak, const Segmentation &segmentation);	

	int getGridID(int iRow, int iCol)
	{
		CHECK_RANGE(iRow, 0, this->gridRows - 1);
		CHECK_RANGE(iCol, 0, this->gridCols - 1);
		return (iRow * this->gridCols) + iCol;
	}

	void getGridRowAndCol(int gridID, int *iRow, int *iCol)
	{
		*iRow = gridID / this->gridCols;
		*iCol = gridID % this->gridCols;

		CHECK_RANGE(*iRow, 0, this->gridRows - 1);
		CHECK_RANGE(*iCol, 0, this->gridCols - 1);
	}
};

#endif
