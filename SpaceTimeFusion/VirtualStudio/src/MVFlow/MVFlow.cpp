#include "MVFlow.h"
#include "ColorHelper.h"
#include "ImageIO.h"

MVFlow::MVFlow(CFloatImage currImage, CFloatImage nextImage)
{
	this->currImg = ColorHelper::RGBtoLuminance(currImage);
	this->nextImg = ColorHelper::RGBtoLuminance(nextImage);

	//ImageProcessing::Operate(this->currImg, this->currImg, 255.0f, ImageProcessing::OP_MUL);
	//ImageProcessing::Operate(this->nextImg, this->nextImg, 255.0f, ImageProcessing::OP_MUL);

	//this->currImg = ImageProcessing::GaussianBlur(this->currImg, 11, 5.0f);
	//this->nextImg = ImageProcessing::GaussianBlur(this->nextImg, 11, 5.0f);
	//ImageIO::WriteFile(this->currImg, "11.tga");
	//ImageIO::WriteFile(this->nextImg, "22.tga");


	//CShape gridShape = this->currImg.Shape();
	//uint nodeAddr = 0;
	//for(int y = 0; y < gridShape.height; y++)
	//for(int x = 0; x < gridShape.width;  x++, nodeAddr++)
	//{
	//	this->currImg[nodeAddr] += (genProb() * 0.03  - 0.015);
	//	this->currImg[nodeAddr] = min(this->currImg[nodeAddr], 1.0f);
	//	this->currImg[nodeAddr] = max(this->currImg[nodeAddr], 0.0f);

	//	this->nextImg[nodeAddr] += (genProb() * 0.03  - 0.015);
	//	this->nextImg[nodeAddr] = min(this->nextImg[nodeAddr], 1.0f);
	//	this->nextImg[nodeAddr] = max(this->nextImg[nodeAddr], 0.0f);
	//}

	//ImageIO::WriteFile(this->currImg, "11.tga");
	//ImageIO::WriteFile(this->nextImg, "22.tga");


	ENSURE(this->currImg.Shape() == this->nextImg.Shape());
}

void MVFlow::ComputeFlow(MVFlowParams mvFlowParams)
{
	//bug blur the input images by the right amount

	this->params = mvFlowParams;

	allocIntermediateData();
	initCurrImgDXY();
	resetUV();
	reset_dU_dV();

	for(int k = 0; k < 500; k++)
	{
		computeDataIterK();		
		ImageIO::WriteFile(this->warpImg, "warp.tga");
		for(int n = 0; n < 10; n++)
		{
			computeGauB_SeidelIter();
			printf(".");

			ImageProcessing::PrintImageStats(this->du);
			ImageProcessing::PrintImageStats(this->dv);

			CFloatImage duVis = this->du.Clone();
			CFloatImage dvVis = this->dv.Clone();


			ImageProcessing::Operate(duVis, duVis, 30.0f, ImageProcessing::OP_MUL);
			ImageProcessing::Operate(dvVis, dvVis, 30.0f, ImageProcessing::OP_MUL);
			ImageProcessing::Operate(duVis, duVis, 128.0f, ImageProcessing::OP_ADD);
			ImageProcessing::Operate(dvVis, dvVis, 128.0f, ImageProcessing::OP_ADD);
			ImageProcessing::Operate(duVis, duVis, 255.0f, ImageProcessing::OP_DIV);
			ImageProcessing::Operate(dvVis, dvVis, 255.0f, ImageProcessing::OP_DIV);
			

			//ImageProcessing::Rescale(duVis, 0.0f, 1.0f);
			//ImageProcessing::Rescale(dvVis, 0.0f, 1.0f);


			ImageIO::WriteFile(duVis, "du.tga");
			ImageIO::WriteFile(dvVis, "dv.tga");
			getchar();
		}
		updateUV();
		printf("\n");

		//ImageProcessing::PrintImageStats(this->u);
		//ImageProcessing::PrintImageStats(this->v);

		CFloatImage uVis = this->u.Clone();
		CFloatImage vVis = this->v.Clone();

		ImageProcessing::Operate(uVis, uVis, 30.0f, ImageProcessing::OP_MUL);
		ImageProcessing::Operate(vVis, vVis, 30.0f, ImageProcessing::OP_MUL);
		ImageProcessing::Operate(uVis, uVis, 128.0f, ImageProcessing::OP_ADD);
		ImageProcessing::Operate(vVis, vVis, 128.0f, ImageProcessing::OP_ADD);
		ImageProcessing::Operate(uVis, uVis, 255.0f, ImageProcessing::OP_DIV);
		ImageProcessing::Operate(vVis, vVis, 255.0f, ImageProcessing::OP_DIV);

		//ImageProcessing::Rescale(uVis, 0.0f, 1.0f);
		//ImageProcessing::Rescale(vVis, 0.0f, 1.0f);
		ImageIO::WriteFile(uVis, "u.tga");
		ImageIO::WriteFile(vVis, "v.tga");
		//if(k % 50 == 0)
		//	getchar();
	}
}

void MVFlow::allocIntermediateData()
{
	CShape gridShape = this->currImg.Shape();

	this->warpImg.ReAllocate(gridShape);

	this->u.ReAllocate(gridShape);
	this->v.ReAllocate(gridShape);
	this->du.ReAllocate(gridShape);
	this->dv.ReAllocate(gridShape);

	this->Ix.ReAllocate(gridShape);
	this->Iy.ReAllocate(gridShape);
	this->Iz.ReAllocate(gridShape);

	this->Ixx.ReAllocate(gridShape);
	this->Ixy.ReAllocate(gridShape);
	this->Iyy.ReAllocate(gridShape);

	this->Ixz.ReAllocate(gridShape);
	this->Iyz.ReAllocate(gridShape);

	this->currImgDX.ReAllocate(gridShape);
	this->currImgDY.ReAllocate(gridShape);

	this->tensorS.ReAllocate(gridShape);
	this->tensorT.ReAllocate(gridShape);

	this->psiDiff_D1.ReAllocate(gridShape);
	this->psiDiff_D2.ReAllocate(gridShape);
	this->psiDiff_S.ReAllocate(gridShape);
}

