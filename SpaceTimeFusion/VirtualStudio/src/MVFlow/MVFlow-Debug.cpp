#include "MVFlow.h"
#include "ImageIO.h"
#include "ImageProcessing.h"


void MVFlow::DumpDebugImgs()
{
	ImageIO::WriteFile(this->currImg, "currImg.tga");
	ImageIO::WriteFile(this->nextImg, "nextImg.tga");
	ImageIO::WriteFile(this->warpImg, "warpImg.tga");

	CFloatImage tempImg;
	
	tempImg = this->Ix.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "Ix.tga");

	tempImg = this->Iy.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "Iy.tga");

	tempImg = this->Iz.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "Iz.tga");

	tempImg = this->Ixx.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "Ixx.tga");

	tempImg = this->Iyy.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "Iyy.tga");

	tempImg = this->Ixy.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "Ixy.tga");

	tempImg = this->Ixz.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "Ixz.tga");

	tempImg = this->Iyz.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "Iyz.tga");

	tempImg = this->currImgDX.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "currImgDX.tga");

	tempImg = this->currImgDY.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "currImgDY.tga");

	tempImg = this->du.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "du.tga");

	tempImg = this->dv.Clone();
	ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
	ImageIO::WriteFile(tempImg, "dv.tga");

	const char *sTensorFNs[9] = {"IxIx", "IxIy", "IxIz",
						         "IyIx", "IyIy", "IyIz",
		                         "IzIx", "IzIy", "IzIz"};

	vector<CFloatImage> Idelta;
	Idelta.push_back(this->Ix.Clone());
	Idelta.push_back(this->Iy.Clone());
	Idelta.push_back(this->Iz.Clone());

	CShape gridShape = this->tensorS.Shape();
	tempImg.ReAllocate(gridShape);
	for(int iRow = 0; iRow < 3; iRow++)
	for(int iCol = 0; iCol < 3; iCol++)
	{
		uint nodeAddr = 0;
		for(int y = 0; y < gridShape.height; y++)
		for(int x = 0; x < gridShape.width;  x++, nodeAddr++)
		{
			tempImg[nodeAddr] = this->tensorS[nodeAddr][iRow][iCol];
		}
		
		string fn;
		fn = sTensorFNs[(iRow * 3) + iCol];
		ImageProcessing::Rescale(tempImg, 0.0f, 1.0f);
		ImageIO::WriteFile(tempImg, fn + ".tga");

		//CFloatImage tempImgDup(gridShape);
		//ImageProcessing::Operate(tempImgDup, Idelta[iRow], Idelta[iCol], ImageProcessing::OP_MUL);
		//ImageProcessing::Rescale(tempImgDup, 0.0f, 1.0f);
		//ImageIO::WriteFile(tempImgDup, fn + "-dup.tga");
	}
}