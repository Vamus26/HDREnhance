#include "Draw.h"


void Draw::DrawPoint(CFloatImage &img, int x, int y, int radius, CVector3 col, bool invertY)
{
	CShape imgShape = img.Shape();

	if(invertY)
		y = imgShape.height - 1 - y;

	int radiusSqr = radius * radius;
	for(int yy = y - radius, yDist = -radius; yy <= y + radius; yy++, yDist++)
	{
		for(int xx = x - radius, xDist = -radius; xx <= x + radius; xx++, xDist++)
		{
			if(imgShape.InBounds(xx, yy) == true)
			{
				if(((xDist * xDist) + (yDist * yDist)) <= radiusSqr)
				{
					img.Pixel(xx, yy, 0) = (float) col[0];
					if(imgShape.nBands == 3)
					{
						img.Pixel(xx, yy, 1) = (float) col[1];
						img.Pixel(xx, yy, 2) = (float) col[2];
					}
				}
			}
		}
	}
}

void Draw::DrawLine(CFloatImage &img, int x1, int y1, int x2, int y2, CVector3 col, int radius, bool invertY)
{
	CShape imgShape = img.Shape();

	if(invertY)
	{
		y2 = imgShape.height - 1 - y2;
		y1 = imgShape.height - 1 - y1;
	}

	int xSpan = abs(x2 - x1);
	int ySpan = abs(y2 - y1);
	int span;

	ENSURE((xSpan != 0) || (ySpan != 0));

	float xIncr = 1.0f;
	float yIncr = 1.0f;

	if(xSpan > ySpan)
	{
		span  = xSpan;
		yIncr = ((float) ySpan / xSpan);
	}
	else
	{
		span = ySpan;
		xIncr = ((float) xSpan / ySpan);
	}

	float x = (float) x1;
	float y = (float) y1;
	if(x2 < x1) xIncr *= -1.0f;
	if(y2 < y1) yIncr *= -1.0f;
	for(int i = 0; i < span; i++)
	{		
		int xx = NEAREST_INT(x);
		int yy = NEAREST_INT(y);
		if(imgShape.InBounds(xx, yy) == true)
		{
			DrawPoint(img, xx, yy, radius, col);
			img.Pixel(xx, yy, 0) = (float) col[0];
			if(imgShape.nBands == 3)
			{
				img.Pixel(xx, yy, 1) = (float) col[1];
				img.Pixel(xx, yy, 2) = (float) col[2];
			}
		}
		x += xIncr;
		y += yIncr;
	}
}

void Draw::DrawBox(CFloatImage &img, int x1, int y1, int x2, int y2, CVector3 col, int radius)
{
	DrawLine(img, x1, y1, x2, y1, col, radius);
	DrawLine(img, x1, y1, x1, y2, col, radius);
	DrawLine(img, x1, y2, x2, y2, col, radius);
	DrawLine(img, x2, y2, x2, y1, col, radius);
}
