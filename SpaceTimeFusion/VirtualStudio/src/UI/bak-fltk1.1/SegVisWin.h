#ifndef __SEG_GRAPH_VIS_WIN_H__
#define __SEG_GRAPH_VIS_WIN_H__

#include "UI_Defs.h"
#include "Segmentation.h"


const static int SEG_VIS_WIN_HistWidth    = 50;
const static int SEG_VIS_WIN_HistElemSize = 1;
const static int SEG_VIS_WIN_Buff         = 20;
const static int SEG_VIS_WIN_LabelHeight  = 15;

class SegVisWin : public Fl_Double_Window
{
	const Segment *seg;
	int depthCount;
	int viewCount;

public:
	SegVisWin();
	void UpdateSegVis(const Segment *newSeg);
	int handle(int eventID);
	void draw();
	void drawVis();
	void drawBarGraph(float *barData, int elemCount, const char *label);

private:
	void resetToNull();
	void resetToNewSeg(const Segment *newSeg);
};

#endif