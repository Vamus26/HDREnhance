#include "UI_Defs.h"


void UI_Defs::DrawRect(int x, int y, int w, int h)
{
	fltk::fillrect(x,y, w, h);
	//fltk::be();
 //   fl_vertex(x, y);
 //   fl_vertex(x + w - 1, y);
 //   fl_vertex(x + w - 1, y + h - 1);
 //   fl_vertex(x, y + h - 1);
 //   fl_end_loop();
}