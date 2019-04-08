#include "SegVisWin.h"

SegVisWin::SegVisWin() : Fl_Double_Window(0,0, "Seg Visualizer")
{	
	this->border(1);
	resetToNull();
}

void SegVisWin::resetToNull()
{
	this->seg = NULL;
	this->depthCount = 0;
	this->viewCount  = 0;
}

void SegVisWin::UpdateSegVis(const Segment *newSeg)
{
	if(newSeg != NULL)
	{
		resetToNewSeg(newSeg);
		this->show();
	}
	else
	{
		resetToNull();
	}
}


void SegVisWin::resetToNewSeg(const Segment *newSeg)
{
	this->seg = newSeg;
	ENSURE(this->seg != NULL);
	ENSURE(this->seg->viewNeighs != NULL);

	this->viewCount = this->seg->viewNeighs->viewCount;
	this->depthCount = this->seg->viewNeighs->depthCount;

	ENSURE(this->viewCount > 0);
	ENSURE(this->depthCount > 0);

	int barGraphCount = this->viewCount + 1; // views + belief
	int newW = 0;
	newW += barGraphCount * SEG_VIS_WIN_HistWidth;
	newW += (barGraphCount + 1) * SEG_VIS_WIN_Buff;

	int newH = 0;
	newH += (2 * SEG_VIS_WIN_Buff) + SEG_VIS_WIN_LabelHeight;
	newH += this->depthCount * SEG_VIS_WIN_HistElemSize;

	this->w(newW);
	this->h(newH);

	damage(FL_DAMAGE_USER1);
}

int SegVisWin::handle(int eventID)
{
	bool handled = false;

	switch(eventID)
	{
	case FL_CLOSE:
		this->hide();
		handled = true;
		break;
	}

	if(handled == true)
		return 1;
	else
		return Fl_Window::handle(eventID);
}


void SegVisWin::draw()
{
	if(damage() != FL_DAMAGE_USER1)
		Fl_Double_Window::draw();

	drawVis();
}

void SegVisWin::drawVis()
{
	if(this->seg != NULL)
	{
		fl_push_matrix();

		fl_translate(SEG_VIS_WIN_Buff, SEG_VIS_WIN_Buff);

		drawBarGraph(this->seg->segData->belief, this->depthCount, "B");
		
		for(int iView = 0; iView < this->viewCount; iView++)
		{			
			char dataLabel[256];
			sprintf(dataLabel, "D-%i", iView);
			fl_translate(SEG_VIS_WIN_Buff + SEG_VIS_WIN_HistWidth, 0);
			drawBarGraph(this->seg->segData->dataCost[iView], this->depthCount, dataLabel);
		}

		fl_pop_matrix();
	}
}

void SegVisWin::drawBarGraph(float *barData, int elemCount, const char *label)
{
	fl_color(0, 0, 0);

	fltk::

	//fl_draw(label, 0, 0, 
	//	    SEG_VIS_WIN_HistWidth, SEG_VIS_WIN_LabelHeight,
	//		FL_ALIGN_CENTER);

	fl_translate(0, SEG_VIS_WIN_LabelHeight);

	for(int iElem = 0; iElem < elemCount; iElem++)
	{
		float elemVal = barData[iElem];
		elemVal *= 255.0f;
		elemVal = std::min(255.0f, std::max(elemVal, 0.0f));

		uchar elemIntensity = (uchar) elemVal;
		fl_color(elemIntensity, elemIntensity, elemIntensity);

		UI_Defs::DrawRect(0, iElem * SEG_VIS_WIN_HistElemSize,
			              SEG_VIS_WIN_HistWidth, SEG_VIS_WIN_HistElemSize);
	}

	fl_translate(0, -SEG_VIS_WIN_LabelHeight);
}
