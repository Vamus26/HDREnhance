#include "VStudioWin.h"
#include "ImageIO.h"
#include "Draw.h"


//check - resizeable, scrollable, scheme, hide

VStudioWin::VStudioWin(int width, int height) : Window(width, height, "Virtual Studio")
{	
	//check - set style	
	Widget::default_style->box_ = fltk::PLASTIC_DOWN_BOX;
	Button::default_style->box_ = fltk::PLASTIC_DOWN_BOX;
	Window::default_style->box_ = fltk::PLASTIC_DOWN_BOX;
	ScrollGroup::default_style->box_ = fltk::PLASTIC_DOWN_BOX;

	this->segVisWin = new SegVisWin();

	begin();
	
	//this->scrollScreen = new ScrollGroup(0,0, width, height);

	this->mainImgBox = new Widget(0,0,0,0);
	ENSURE(this->mainImgBox != NULL);
	this->mainImgFL = NULL;

	this->auxImgBox = new Widget(0,0,0,0);
	ENSURE(this->auxImgBox != NULL);
	this->auxImgFL = NULL;
	
	end();

	//this->resizable(this);
}

VStudioWin::~VStudioWin()
{
	if(!this->mainImgBox)
		delete this->mainImgBox;
	
	if(!this->mainImgFL)
		delete this->mainImgFL;

	if(!this->segVisWin)
		delete this->segVisWin;

	if(!this->auxImgBox)
		delete this->auxImgBox;
	
	if(!this->auxImgFL)
		delete this->auxImgFL;
}

int VStudioWin::Start(OptFileParser::OptFileParams appParams)
{
	this->params = appParams;
	this->stereoProc.Init(this->params.stereoParams, this->params.rigParams);	
	
	show();

	return fltk::run();
}

void VStudioWin::updateWidgetImg(Widget *widget, rgbImage **imgFL,
								 const CByteImage &newImage)
{
	widget->deactivate();

	CShape newImgShape = newImage.Shape();

	widget->resize(widget->x(), widget->y(), newImgShape.width, newImgShape.height);	
	
	if(*imgFL != NULL)
	{
		delete *imgFL;
	}
	*imgFL = new rgbImage((uchar *) newImage.PixelAddress(0, 0, 0),
						  RGB, newImgShape.width, newImgShape.height);	
	widget->image(*imgFL);

	widget->activate();
	widget->redraw();
	widget->show();
}

void VStudioWin::updateMainImg(CByteImage newMainImg)
{	
	newMainImg = ImageProcessing::FlipVertical(newMainImg);
	CShape imgShape = newMainImg.Shape();
	if(imgShape.nBands != 3)
	{
		ENSURE(imgShape.nBands == 1);
		imgShape.nBands = 3;
		this->mainImg.ReAllocate(imgShape);
		for(int channel = 0; channel < imgShape.nBands; channel++)
		{
			ImageProcessing::CopyChannel(this->mainImg, newMainImg, channel, 0);
		}
	}
	else
	{
		this->mainImg = newMainImg;
	}

	//bug - change all fl widgets names to nameFL
	updateWidgetImg(this->mainImgBox, &this->mainImgFL, this->mainImg);
}

void VStudioWin::updateAuxImg(CByteImage newAuxImg)
{	
	newAuxImg = ImageProcessing::FlipVertical(newAuxImg);
	CShape imgShape = newAuxImg.Shape();
	if(imgShape.nBands != 3)
	{
		ENSURE(imgShape.nBands == 1);
		imgShape.nBands = 3;
		this->auxImg.ReAllocate(imgShape);
		for(int channel = 0; channel < imgShape.nBands; channel++)
		{
			ImageProcessing::CopyChannel(this->auxImg, newAuxImg, channel, 0);
		}
	}
	else
	{
		this->auxImg = newAuxImg;
	}

	this->auxImgBox->x(this->mainImgBox->w() + 10);
	updateWidgetImg(this->auxImgBox, &this->auxImgFL, this->auxImg);
}

int VStudioWin::handle(int eventID)
{
	bool handled = false;

	switch(eventID) 
	{
	case SHORTCUT:
		if(fltk::event_key() == RightKey)
		{
			handleStep();
			handled = true;
		}
		else if(fltk::event_key() == PageUpKey)
		{
			handleSwitchToColorVis();
			handled = true;
		}
		else if(fltk::event_key() == PageDownKey)
		{
			handleSwitchToDepthVis();
			handled = true;
		}

		break;

	case PUSH:
		if(fltk::event_button() == LeftButton)
		{
			handleSegVis(); 
			handled = true;
		}
		if(fltk::event_button() == RightButton)
		{
			handleMatchVis(0); 
			handled = true;
		}
		break;
	}

	if(handled == true)
		return 1;
	else
		return Window::handle(eventID);
}

void VStudioWin::handleStep()
{
	hideCurrContextWins();
	this->stereoProc.Step();
	this->planeMap = this->stereoProc.GetCurrViewPlaneIndexImage();
	this->depthVis = this->stereoProc.GetDepthsVis(this->planeMap);
	updateMainImg(ImageProcessing::FloatToByteImg(this->depthVis));
}

void VStudioWin::hideCurrContextWins()
{
	this->segVisWin->hide();
	this->segVisWin->UpdateSegVis(NULL);
}

void VStudioWin::handleSegVis()
{
	//check - scrollable
	//int x = fltk::event_x() + this->scrollScreen->xposition();
	//int y = fltk::event_y() + this->scrollScreen->yposition();

	int x = fltk::event_x();
	int y = fltk::event_y();

	if(isPointInWidget(this->mainImgBox, x, y))
	{
		y = this->mainImgBox->h() - y - 1;
		
		const CIntImage &labelImg = this->stereoProc.currView->seg->labelImg;
		const vector<Segment> &segments = this->stereoProc.currView->seg->segments;

		int segLabel = labelImg.Pixel(x, y, 0);
		const Segment &currSeg = segments[segLabel];

		this->segVisWin->UpdateSegVis(&currSeg);
	}
}

void VStudioWin::handleMatchVis(int iView)
{
	CHECK_RANGE(iView, 0, (int) this->stereoProc.neighViews.size() - 1);

	CVector3 col(1.0, 0.0, 0.0);

	int x = fltk::event_x();
	int y = fltk::event_y();
	if(isPointInWidget(this->mainImgBox, x, y))
	{
		CShape maskShape = this->planeMap.Shape();
		ENSURE(maskShape.InBounds(x, y));
		y = maskShape.height - y - 1;		
		
		const CameraMats &camMats = this->stereoProc.currView->cam->cameraMats;
		const CameraMats &neighMats = this->stereoProc.neighViews[iView].cam->cameraMats;
		int currPlaneIndex = this->planeMap.Pixel(x, y, 0);
		CHECK_RANGE(currPlaneIndex, 0, this->stereoProc.planeCount - 1);

		const Plane &currPlane = this->stereoProc.planes[currPlaneIndex];

		CVector2 currUV(x, y);
		CVector3 worldXYZ;
		camMats.projUVtoXYZ(currUV, currPlane, worldXYZ);

		CVector2 neighUV;
		neighMats.projXYZtoUV(worldXYZ, neighUV);

		CFloatImage neighImg = this->stereoProc.neighViews[iView].smoothImg.Clone();
		if(neighImg.Shape().InBounds(neighUV[0], neighUV[1]))
		{
			Draw::DrawPoint(neighImg, NEAREST_INT(neighUV[0]), NEAREST_INT(neighUV[1]), 2, col);
			updateAuxImg(ImageProcessing::FloatToByteImg(neighImg));
			//ImageIO::WriteFile(neighImg, "1.tga");
		}
		else
		{
			printf("Projection out of bounds");
		}
	}
}

void VStudioWin::handleSwitchToColorVis()
{
	updateMainImg(ImageProcessing::FloatToByteImg(this->depthVis));
}

void VStudioWin::handleSwitchToDepthVis()
{
	CFloatImage colVis = this->stereoProc.currView->smoothImg;
	updateMainImg(ImageProcessing::FloatToByteImg(colVis));
}


bool VStudioWin::isPointInWidget(Widget *widget, int x, int y)
{
	if((x < widget->x()) ||
	   (y < widget->y()) ||
	   (x >= widget->x() + widget->w()) ||
	   (y >= widget->y() + widget->h()))
	{
		return false;
	}
	else
	{
		return true;
	}
}