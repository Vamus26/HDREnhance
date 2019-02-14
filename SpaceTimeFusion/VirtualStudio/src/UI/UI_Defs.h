#ifndef __UI_DEFS_H__
#define __UI_DEFS_H__

#include "Definitions.h"

#pragma warning(push)
#pragma warning(disable : 4321 4311 4312)
#include <fltk/x.h>
#include <fltk/run.h>
#include <fltk/draw.h>
#include <fltk/style.h>
#include <fltk/events.h>
#include <fltk/layout.h>
#include <fltk/damage.h>
#include <fltk/rgbImage.h>
#include <fltk/Box.h>
#include <fltk/Button.h>
#include <fltk/ScrollGroup.h>
#include <fltk/Window.h>
#pragma warning(pop)

using namespace fltk;


class UI_Defs
{
public:
	static void DrawRect(int x, int y, int w, int h);


};

#endif