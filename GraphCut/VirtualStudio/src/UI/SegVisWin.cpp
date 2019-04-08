#include "SegVisWin.h"

SegVisWin::SegVisWin() : Window(0,0, "Seg Visualizer")
{	
	set_double_buffer();
	this->border(1);
	resetToNull();
}

void SegVisWin::resetToNull()
{
	this->seg = NULL;
	this->planeCount = 0;
	this->viewCount  = 0;
}

void SegVisWin::UpdateSegVis(const Segment *newSeg)
{
	if(newSeg != NULL)
	{
		resetToNewSeg(newSeg);		
		show();
		//check - use hide feature, fltk broken?
		//if(!shown())
		//	show();
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
	this->planeCount = this->seg->viewNeighs->planeCount;

	ENSURE(this->viewCount > 0);
	ENSURE(this->planeCount > 0);

	int barGraphCount =  (2 * this->viewCount) + 2; // views + belief
	int newW = 0;
	newW += barGraphCount * SEG_VIS_WIN_HistWidth;
	newW += (barGraphCount + 1) * SEG_VIS_WIN_Buff;

	int newH = 0;
	newH += (2 * SEG_VIS_WIN_Buff) + SEG_VIS_WIN_LabelHeight;
	newH += this->planeCount * SEG_VIS_WIN_HistElemSize;

	this->w(newW);
	this->h(newH);

	redraw(SEG_VIS_WIN_Damage_Vis);
}

int SegVisWin::handle(int eventID)
{
	bool handled = false;

	//check - use hide, fltk broken?
	//switch(eventID)
	//{
	//case HIDE:
	//	this->hide();
	//	handled = true;
	//	break;
	//}

	if(handled == true)
		return 1;
	else
		return Window::handle(eventID);
}


void SegVisWin::draw()
{
	int damageType = damage();
	 
	if(damageType & fltk::DAMAGE_ALL)
		Window::draw();

	if(damageType & (fltk::DAMAGE_ALL | SEG_VIS_WIN_Damage_Vis)) 
		drawVis();
}

void SegVisWin::drawVis()
{
	if(this->seg != NULL)
	{
		fltk::push_matrix();

		fltk::translate(SEG_VIS_WIN_Buff, SEG_VIS_WIN_Buff);
		drawBarGraph(this->seg->segData->belief, this->planeCount, "B");

		fltk::translate(SEG_VIS_WIN_Buff + SEG_VIS_WIN_HistWidth, 0);
		drawBarGraph(this->seg->segData->notOccProb, this->viewCount, "NO");
		
		for(int iView = 0; iView < this->viewCount; iView++)
		{			
			char label[256];
			
			sprintf(label, "D-%i", iView);
			fltk::translate(SEG_VIS_WIN_Buff + SEG_VIS_WIN_HistWidth, 0);
			drawBarGraph(this->seg->segData->dataCost[iView], this->planeCount, label);

			sprintf(label, "P-%i", iView);
			fltk::translate(SEG_VIS_WIN_Buff + SEG_VIS_WIN_HistWidth, 0);
			drawBarGraph(this->seg->viewNeighs->projBelief[iView], this->planeCount, label);
		}

		fltk::pop_matrix();
	}

	printf("\n");
}

void SegVisWin::drawBarGraph(float *barData, int elemCount, const char *label)
{
	fltk::setcolor(fltk::color(0, 0, 0));
	Rectangle labelBounds(SEG_VIS_WIN_HistWidth, SEG_VIS_WIN_LabelHeight);
	fltk::drawtext(label, labelBounds, fltk::ALIGN_CENTER);

	fltk::translate(0, SEG_VIS_WIN_LabelHeight);

	float sum = 0.0;
	for(int iElem = 0; iElem < elemCount; iElem++)
	{
		float elemVal = barData[iElem];

		sum += elemVal;

		elemVal *= 255.0f;
		elemVal = std::min(255.0f, std::max(elemVal, 0.0f));

		uchar elemIntensity = (uchar) elemVal;
		fltk::setcolor(fltk::color(elemIntensity, elemIntensity, elemIntensity));

		UI_Defs::DrawRect(0, iElem * SEG_VIS_WIN_HistElemSize,
			              SEG_VIS_WIN_HistWidth, SEG_VIS_WIN_HistElemSize);
	}

	PRINT_FLOAT(sum);

	fltk::translate(0, -SEG_VIS_WIN_LabelHeight);
}
