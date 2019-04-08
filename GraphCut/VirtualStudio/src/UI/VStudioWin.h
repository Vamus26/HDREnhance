#ifndef __VStudioWin_H__
#define __VStudioWin_H__

#include "UI_Defs.h"
#include "OptFileParser.h"
#include "Stereo.h"
#include "SegVisWin.h"

class VStudioWin : public Window
{
private:
	OptFileParser::OptFileParams params;

	SegVisWin *segVisWin;

	ScrollGroup *scrollScreen;

	CByteImage mainImg;
	Widget *mainImgBox;
	rgbImage *mainImgFL;


	CByteImage auxImg;
	Widget *auxImgBox;
	rgbImage *auxImgFL;


	CFloatImage depthVis;
	CShortImage planeMap;

	Stereo stereoProc;

public: //VStudioWin.cpp
	VStudioWin(int width = 2000, int height = 1000);
	~VStudioWin();

	int Start(OptFileParser::OptFileParams appParams);

	int handle(int eventID);

private: //VStudioWin.cpp
	void updateMainImg(CByteImage newMainImg);
	void updateAuxImg(CByteImage newAuxImg);
	void updateWidgetImg(Widget *widget, rgbImage **imgFl,
					     const CByteImage &newImage);

	void handleStep();
	void handleSwitchToColorVis();
	void handleSwitchToDepthVis();
	void handleSegVis(); 
	void handleMatchVis(int iView);

	bool isPointInWidget(Widget *widget, int x, int y);

	void hideCurrContextWins();
};


#endif 