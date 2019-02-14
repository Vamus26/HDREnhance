#ifndef __VStudioWin_H__
#define __VStudioWin_H__

#include "UI_Defs.h"
#include "Fl_Scroll.h"
#include "OptFileParser.h"
#include "Stereo.h"
#include "SegVisWin.h"

class VStudioWin : public Fl_Window
{
private:
	OptFileParser::OptFileParams params;

	SegVisWin *segVisWin;

	Fl_Scroll *scrollScreen;

	CByteImage mainImg;
	Fl_Box *mainImgBox;
	Fl_RGB_Image *mainImgFL;

	Fl_Box *segGraphBox;
	Fl_RGB_Image *segGraphImgFL; 
	CByteImage segGraphImg;

	Stereo stereoProc;

public: //VStudioWin.cpp
	VStudioWin(int width = 1200, int height = 800);
	~VStudioWin();

	int Start(OptFileParser::OptFileParams appParams);

	int handle(int eventID);

private: //VStudioWin.cpp
	void updateMainImg(CByteImage newMainImg);
	void updateBoxImg(Fl_Box *boxFL, Fl_RGB_Image **imgFl,
					  const CByteImage &newImage);

	void handleStep();
	void handleSegVis(); 

	bool isPointInWidget(Fl_Widget *widget, int x, int y);

	void hideCurrContextWins();
};


#endif 