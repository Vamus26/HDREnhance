#ifndef __DRAW_H__
#define __DRAW_H__

#include "Definitions.h"
#include "Image.h"
#include "Transform-3x3.h"

class Draw
{
public:
	static void DrawPoint(CFloatImage &img, int x, int y, int radius, CVector3 col, bool invertY = false);

	static void DrawLine(CFloatImage &img, int x1, int y1, int x2, int y2, CVector3 col, int radius = 0, bool invertY = false);

	static void DrawBox(CFloatImage &img, int x1, int y1, int x2, int y2, CVector3 col, int radius = 0);
};

#endif
