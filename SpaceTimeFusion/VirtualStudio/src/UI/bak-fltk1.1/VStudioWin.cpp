#include "VStudioWin.h"
#include "ImageIO.h"

VStudioWin::VStudioWin(int width, int height) : Fl_Window(width, height, "Virtual Studio")
{
	Fl::scheme("plastic");

	this->segVisWin = new SegVisWin();

	begin();		

	this->scrollScreen = new Fl_Scroll(0,0, width, height);

	this->mainImgBox = new Fl_Box(0,0,0,0);
	ENSURE(this->mainImgBox != NULL);
	this->mainImgFL = NULL;
	
	CShape segGraphShape(200, 500, 3); //bug - change 500 to depths
	this->segGraphImg.ReAllocate(segGraphShape);
	this->segGraphBox = new Fl_Box(width  - segGraphShape.width - 1,
				                   0,
						 		   segGraphShape.width,
							       segGraphShape.height);
	this->segGraphImgFL = NULL;


	
	end();

	this->resizable(this);
}

VStudioWin::~VStudioWin()
{
	if(!this->mainImgBox)
		delete this->mainImgBox;
	
	if(!this->mainImgFL)
		delete this->mainImgFL;

	if(!this->segGraphBox)
		delete this->segGraphBox;

	if(!this->segGraphImgFL)
		delete this->segGraphBox;

	if(!this->segVisWin)
		delete this->segVisWin;
}


int VStudioWin::Start(OptFileParser::OptFileParams appParams)
{
	this->params = appParams;
	this->stereoProc.Init(this->params.stereoParams, this->params.rigParams);	
	
	show();

	return Fl::run();
}

void VStudioWin::updateBoxImg(Fl_Box *boxFL, Fl_RGB_Image **imgFL,
							  const CByteImage &newImage)
{
	boxFL->deactivate();

	CShape newImgShape = newImage.Shape();
	
	if(*imgFL != NULL)
	{
		delete *imgFL;
	}
	*imgFL = new Fl_RGB_Image((uchar *) newImage.PixelAddress(0, 0, 0),
						      newImgShape.width, newImgShape.height);
	
	boxFL->image(*imgFL);
	boxFL->resize(boxFL->x(), boxFL->y(), newImgShape.width, newImgShape.height);	
	boxFL->activate();
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

	//bug - change all fl widgets names to nameFL
	updateBoxImg(this->mainImgBox, &this->mainImgFL, this->mainImg);
}

int VStudioWin::handle(int eventID)
{
	bool handled = false;

	switch(eventID) 
	{
	case FL_KEYUP:
		if(Fl::event_key() == FL_Right)
		{
			handleStep();
			handled = true;
		}
		break;

	case FL_RELEASE:
		handleSegVis(); 
		handled = true;
		break;
	}

	if(handled == true)
		return 1;
	else
		return Fl_Window::handle(eventID);
}

void VStudioWin::handleStep()
{
	hideCurrContextWins();
	this->stereoProc.Step();
	CFloatImage depthVis = this->stereoProc.GetCurrViewDepthsVis();
	updateMainImg(ImageProcessing::FloatToByteImg(depthVis));
}

void VStudioWin::hideCurrContextWins()
{
	this->segVisWin->hide();
	this->segVisWin->UpdateSegVis(NULL);
}

void VStudioWin::handleSegVis()
{
	int x = Fl::event_x() + this->scrollScreen->xposition();
	int y = Fl::event_y() + this->scrollScreen->yposition();

	if(isPointInWidget(this->mainImgBox, x, y))
	{
		y = this->mainImgBox->h() - y - 1;
		
		const CIntImage &labelImg = this->stereoProc.currView->seg->labelImg;
		const vector<Segment> &segments = this->stereoProc.currView->seg->segments;

		int segLabel = labelImg.Pixel(x, y, 0);
		const Segment &currSeg = segments[segLabel];

		this->segVisWin->UpdateSegVis(&currSeg);
		this->segVisWin->show();

		int depthCount = this->stereoProc.currView->seg->depthCount;
		ENSURE(depthCount > 0);

		for(int iDepth = 0; iDepth < depthCount; iDepth++)
		{
			PRINT_FLOAT(currSeg.segData->belief[iDepth]);
		}
		printf("\n");
		for(int iDepth = 0; iDepth < depthCount; iDepth++)
		{
			PRINT_FLOAT(currSeg.segData->dataCost[0][iDepth]);
		}

	
		this->segGraphImg.ClearPixels();
		ImageProcessing::Operate(this->segGraphImg, this->segGraphImg, (uchar) 255, ImageProcessing::OP_SET);
		for(int iDepth = 0; iDepth < depthCount; iDepth++)
		{
			float currBelief = currSeg.segData->belief[iDepth];
			currBelief *= 255.0f;		
			currBelief = std::min(255.0f, std::max(currBelief, 0.0f));
			
			//bug - use all views
			float currDatacost = currSeg.segData->dataCost[0][iDepth];
			currDatacost *= 255.0f;
			currDatacost = std::min(255.0f, std::max(currDatacost, 0.0f));			

			CShape segGraphShape = this->segGraphImg.Shape();
			for(int visX = 0; visX < 90; visX++)
			{
				for(int channel = 0; channel < segGraphShape.nBands; channel++)
				{
					if(channel == 0)
					{
						this->segGraphImg.Pixel(visX, iDepth, channel)       = (uchar) currBelief;
						this->segGraphImg.Pixel(visX + 100, iDepth, channel) = (uchar) currDatacost;
					}
					else
					{
						this->segGraphImg.Pixel(visX, iDepth, channel)       = 0;
						this->segGraphImg.Pixel(visX + 100, iDepth, channel) = 0;
					}
				}
			}
		}
		printf("\n");

		updateBoxImg(this->segGraphBox, &this->segGraphImgFL, this->segGraphImg);
		
	}
	//this->mainImgBox->x
}


bool VStudioWin::isPointInWidget(Fl_Widget *widget, int x, int y)
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