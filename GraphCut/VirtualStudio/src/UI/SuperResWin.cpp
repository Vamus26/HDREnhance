#include "SuperResWin.h"
#include "ImageIO.h"
#include "Timer.h"
#include "Draw.h"

SuperResWin::SuperResWin(int width, int height) : Window(width, height, "SuperRes")
{	
	//check - set style	
	Widget::default_style->box_ = fltk::PLASTIC_DOWN_BOX;
	Button::default_style->box_ = fltk::PLASTIC_DOWN_BOX;
	Window::default_style->box_ = fltk::PLASTIC_DOWN_BOX;
	ScrollGroup::default_style->box_ = fltk::PLASTIC_DOWN_BOX;

	begin();
	
	//this->scrollScreen = new ScrollGroup(0,0, width, height);

	this->mainImgBox = new Widget(0,0,0,0);
	ENSURE(this->mainImgBox != NULL);
	this->mainImgFL = NULL;

	this->secondImgBox = new Widget(0,0,0,0);
	ENSURE(this->secondImgBox != NULL);
	this->secondImgFL = NULL;


	this->superResProc = NULL;
	
	end();
}

SuperResWin::~SuperResWin()
{
	if(!this->mainImgBox)
		delete this->mainImgBox;
	
	if(!this->mainImgFL)
		delete this->mainImgFL;

	if(!this->superResProc)
		delete this->superResProc;
}


int SuperResWin::Start(int args, char *argv[])
{
	if(args < 3)
	{
		REPORT_FAILURE("Usage: training-img-count training-img target-img\n");
		exit(0);
	}

	this->superResParams = SuperRes::SuperResParams::Default();
	this->superResParams.outPrefix = "test";
	this->superResProc = new SuperRes(this->superResParams);

	vector<CFloatImage> trainingImages;
	int trainingImgCount = atoi(argv[1]);
	ENSURE(trainingImgCount == 1);
	for(int iData = 0; iData < trainingImgCount; iData++)
	{
		CFloatImage trainingImg;
		ImageIO::ReadFile(trainingImg, argv[2 + iData]);
		trainingImg = SuperRes::GetCroppedImg(trainingImg, this->superResParams.downSampleScale);
		trainingImages.push_back(trainingImg);
	}

	this->superResProc->TrainSuperRes(trainingImages);
	
	CFloatImage targetImg;
	ImageIO::ReadFile(targetImg, this->superResParams.outPrefix + ".tga");

	this->superResProc->SetTargetImg(targetImg);

	updateMainImg(this->superResProc->aImgsOrg[0]);
	updateSecondImg(this->superResProc->targetImgs[0]);

	show();

	return fltk::run();
}



void SuperResWin::updateWidgetImg(Widget *widget, rgbImage **imgFL,
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

void SuperResWin::updateMainImg(CFloatImage newMainImg)
{	
	updateMainImg(ImageProcessing::FloatToByteImg(newMainImg));
}


void SuperResWin::updateMainImg(CByteImage newMainImg)
{	
	this->mainImg = ImageProcessing::FlipVertical(newMainImg);
	updateWidgetImg(this->mainImgBox, &this->mainImgFL, this->mainImg);
}

void SuperResWin::updateSecondImg(CFloatImage newMainImg)
{	
	updateSecondImg(ImageProcessing::FloatToByteImg(newMainImg));
}


void SuperResWin::updateSecondImg(CByteImage newMainImg)
{	
	this->secondImg = ImageProcessing::FlipVertical(newMainImg);
	this->secondImgBox->x(this->mainImgBox->w() + 10);
	this->secondImgBox->y(0);
	updateWidgetImg(this->secondImgBox, &this->secondImgFL, this->secondImg);
}


int SuperResWin::handle(int eventID)
{
	bool handled = false;

	switch(eventID) 
	{		
	case SHORTCUT:
		if(fltk::event_key() == F1Key)
		{
			Timer timer(true);
			printf("Generating labels...\n");
			this->superResProc->generateLabels(0);
			timer.DumpSeconds("\n  Done.");
			printf("\nComputing datacosts...\n");
			this->superResProc->computeDataCosts(0);
			timer.DumpSeconds("\n  Done.");
			printf("\nComputing spatial smoothness data...\n");
			this->superResProc->computeSpatialSmoothnessData(0);
			timer.DumpSeconds("\n  Done.");

			handled = true;
		}
		else if(fltk::event_key() == F2Key)
		{
			Timer timer(true);
			printf("Computing BP iteration...\n");
			this->superResProc->bpIter(0);
			timer.DumpSeconds("\n  Done.");

			handled = true;
		}

		break;

	case PUSH:
		if(fltk::event_button() == LeftButton)		
		if(isPointInWidget(this->secondImgBox, fltk::event_x(), fltk::event_y()) == true)
		{
			if(fltk::event_key_state(TabKey) == true)
			{
				drawLabels(this->superResParams.maxLabels);			
				handled = true;
			}
			else if(fltk::event_key_state(49) == true)
			{
				drawLabels(1);
				handled = true;
			}
			else if(fltk::event_key_state(50) == true)
			{
				drawLabels(2);
				handled = true;
			}
			else if(fltk::event_key_state(51) == true)
			{
				drawLabels(3);
				handled = true;
			}
			if(fltk::event_key_state(LeftShiftKey) == true)
			{
				drawBestLabel();
				handled = true;
			}
		}
		break;

	}

	if(handled == true)
		return 1;
	else
		return Window::handle(eventID);
}

void SuperResWin::drawLabels(int maxLabels)
{
	int pixelShift = (this->superResParams.patchSize - this->superResParams.patchOverlap);

	int largePatchHalfWidth = this->superResParams.largePatchSize / 2;

	int x = fltk::event_x() - this->secondImgBox->x();
	int y = this->secondImgBox->h() - 1 - fltk::event_y();

	int xx = x / pixelShift;
	int yy = y / pixelShift;

	CFloatImage rightImg = this->superResProc->targetImgs[0].Clone();

	CVector3 col(1.0f, 0.0f, 0.0f);
	Draw::DrawBox(rightImg, 
				   xx * pixelShift - largePatchHalfWidth,
				   yy * pixelShift - largePatchHalfWidth,
				   xx * pixelShift + largePatchHalfWidth,
				   yy * pixelShift + largePatchHalfWidth,
				   col);

	updateSecondImg(rightImg);

	CFloatImage leftImg = this->superResProc->aImgsOrg[0].Clone();

	for(int i = 0; i < maxLabels; i++)
	{
		int label = this->superResProc->labels[0].Pixel(xx, yy, 0)[i];
		int patchImgID, patchImgX, patchImgY;
		this->superResProc->getPatchLoc(label, patchImgX, patchImgY, patchImgID);
		ENSURE(patchImgID == 0);

		col[0] = 1.0f - ((float) i / this->superResParams.maxLabels);

		Draw::DrawBox(leftImg, 
					   patchImgX - largePatchHalfWidth,
					   patchImgY - largePatchHalfWidth,
					   patchImgX + largePatchHalfWidth,
					   patchImgY + largePatchHalfWidth,
					   col);
	}

	updateMainImg(leftImg);
}


void SuperResWin::drawBestLabel()
{
	int pixelShift = (this->superResParams.patchSize - this->superResParams.patchOverlap);

	int largePatchHalfWidth = this->superResParams.largePatchSize / 2;

	int x = fltk::event_x() - this->secondImgBox->x();
	int y = this->secondImgBox->h() - 1 - fltk::event_y();

	int xx = x / pixelShift;
	int yy = y / pixelShift;

	CFloatImage rightImg = this->superResProc->targetImgs[0].Clone();

	CVector3 col(1.0f, 0.0f, 0.0f);
	Draw::DrawBox(rightImg, 
				   xx * pixelShift - largePatchHalfWidth,
				   yy * pixelShift - largePatchHalfWidth,
				   xx * pixelShift + largePatchHalfWidth,
				   yy * pixelShift + largePatchHalfWidth,
				   col);

	updateSecondImg(rightImg);

	CFloatImage leftImg = this->superResProc->aImgsOrg[0].Clone();	

	int label = this->superResProc->getLabel(0, this->superResProc->labelShape.NodeIndex(xx, yy));
	int patchImgID, patchImgX, patchImgY;
	this->superResProc->getPatchLoc(label, patchImgX, patchImgY, patchImgID);
	ENSURE(patchImgID == 0);

	PRINT_INT(label == this->superResProc->labels[0].Pixel(xx, yy, 0)[0]);


	Draw::DrawBox(leftImg, 
				   patchImgX - largePatchHalfWidth,
				   patchImgY - largePatchHalfWidth,
				   patchImgX + largePatchHalfWidth,
				   patchImgY + largePatchHalfWidth,
				   col);

	updateMainImg(leftImg);
}


bool SuperResWin::isPointInWidget(Widget *widget, int x, int y)
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