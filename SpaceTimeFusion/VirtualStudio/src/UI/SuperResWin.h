#ifndef __SuperResWin_H__
#define __SuperResWin_H__

#include "UI_Defs.h"
#include "Image.h"
#include "SuperRes.h"

class SuperResWin : public Window
{
private:

	ScrollGroup *scrollScreen;

	CByteImage mainImg;
	Widget *mainImgBox;
	rgbImage *mainImgFL;

	CByteImage secondImg;
	Widget *secondImgBox;
	rgbImage *secondImgFL;

	SuperRes::SuperResParams superResParams;
	SuperRes *superResProc;	

public: //VStudioWin.cpp
	SuperResWin(int width = 1200, int height = 800);
	~SuperResWin();

	int Start(int args, char *argv[]);

	int handle(int eventID);

private: //VStudioWin.cpp
	void updateMainImg(CByteImage newMainImg);
	void updateMainImg(CFloatImage newMainImg);
	void updateSecondImg(CByteImage newMainImg);
	void updateSecondImg(CFloatImage newMainImg);

	void updateWidgetImg(Widget *widget, rgbImage **imgFl,
					     const CByteImage &newImage);

	void drawLabels(int maxLabels);
	bool isPointInWidget(Widget *widget, int x, int y);


	void drawBestLabel();
};


#endif 