#ifndef __UI_DEFS_H__
#define __UI_DEFS_H__

#include "Definitions.h"

#pragma warning(push)
#pragma warning(disable : 4321 4311 4312)
#include <Fl.H>
#include <Fl_Window.H>
#include <Fl_Double_Window.H>
#include <Fl_Box.H>
#include <Fl_Image.H>
#include <Fl_Draw.H>
#pragma warning(pop)





class UI_Defs
{
public:
	static void DrawRect(int x, int y, int w, int h);


};

#endif