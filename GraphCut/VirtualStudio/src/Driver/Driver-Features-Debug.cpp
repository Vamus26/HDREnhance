#include "Driver.h"
#include "Disk.h"
#include "Features.h"
#include "FeatureRenderer.h"
#include "FeatureMatcher.h"
#include "Motion.h"

void Driver::renderFeatures()
{
	ENSURE(this->params.genericParams.strList1.size() > 0);
	ENSURE(this->params.genericParams.strList2.size() == 0);
	ENSURE(this->params.genericParams.intList1.size() == 0);
	ENSURE(this->params.genericParams.intList2.size() == 0);
	ENSURE((this->params.genericParams.floatList.size() == 0) ||
		   (this->params.genericParams.floatList.size() == 1));

	string outDir = "temp";
	Disk::MakeDir(outDir);

	float renderProb = 1.0f;
	if(this->params.genericParams.floatList.size() == 1)
	{
		renderProb = this->params.genericParams.floatList[0];
	}

	for(vector<string>::const_iterator currImgID = this->params.genericParams.strList1.begin();
		currImgID != this->params.genericParams.strList1.end();
		currImgID++)
	{
		printf("Combining features - %s\n", currImgID->c_str());

		FeatureSet currImgFeaSet;
		loadFeatureSet(*currImgID, currImgFeaSet);
		PRINT_INT(currImgFeaSet.size());

		CFloatImage currImg;
		ImageIO::ReadFile(currImg, *currImgID + CAMERA_ImageExt);

		FeatureRenderer::RenderFeatureSet(currImg, currImgFeaSet, renderProb);
		string outFN = outDir + OS_PATH_DELIMIT_STR + Disk::GetBaseFileName(*currImgID) + "-fea.tga";
		ImageIO::WriteFile(currImg, outFN);
	
		printf("\tDone\n\n");
	}
}


void Driver::renderFeatureMatches()
{
	ENSURE(this->params.genericParams.strList1.size() > 0);
	ENSURE(this->params.genericParams.strList1.size() ==
		   this->params.genericParams.strList2.size());
	ENSURE((this->params.genericParams.floatList.size() == 0) ||
		   (this->params.genericParams.floatList.size() == 1));
	ENSURE(this->params.genericParams.intList1.size() == 0);
	ENSURE(this->params.genericParams.intList2.size() == 0);


	string outDir = "temp";
	Disk::MakeDir(outDir);

	float renderProb = 1.0f;
	if(this->params.genericParams.floatList.size() == 1)
	{
		renderProb = this->params.genericParams.floatList[0];
	}

	for(vector<string>::const_iterator img1ID = this->params.genericParams.strList1.begin(),
		                               img2ID = this->params.genericParams.strList2.begin();
		img1ID != this->params.genericParams.strList1.end();
		img1ID++, img2ID++)
	{	
		printf("Computing matches - (%s,%s) \n", img1ID->c_str(), img2ID->c_str());

		FeatureSet img1FeaSet, img2FeaSet;
		FeatureMatcher::FeatureMatcherParams feaMatcherParams = this->params.feaMatcherParams;
		feaMatcherParams.loadFromFileIfAvaliable = true;
		feaMatcherParams.writeToFile = true;
		FeatureMatches feaMatches;
		loadFeatureMatches(*img1ID, *img2ID, 
						    img1FeaSet, img2FeaSet,
							feaMatches, feaMatcherParams);
		PRINT_INT(feaMatches.size());

		CFloatImage img1, img2;
		ImageIO::ReadFile(img1, *img1ID + CAMERA_ImageExt);
		ImageIO::ReadFile(img2, *img2ID + CAMERA_ImageExt);

		CFloatImage resultImg = FeatureRenderer::RenderFeatureMatches(img1, img2, img1FeaSet, img2FeaSet, feaMatches, renderProb); 
		string outFN = outDir + OS_PATH_DELIMIT_STR + Disk::GetBaseFileName(*img1ID) + "-" + Disk::GetBaseFileName(*img2ID) + ".tga";
		ImageIO::WriteFile(resultImg, outFN);

		printf("\tDone\n\n");
	}
}


void Driver::renderFM_Inliers()
{
	ENSURE(this->params.genericParams.strList1.size() > 0);
	ENSURE(this->params.genericParams.strList1.size() ==
		   this->params.genericParams.strList2.size());
	ENSURE(this->params.genericParams.intList1.size() == 0);
	ENSURE(this->params.genericParams.intList2.size() == 0);
	ENSURE((this->params.genericParams.floatList.size() == 0) ||
		   (this->params.genericParams.floatList.size() == 1));

	string outDir = "temp";
	Disk::MakeDir(outDir);

	float renderProb = 1.0f;
	if(this->params.genericParams.floatList.size() == 1)
	{
		renderProb = this->params.genericParams.floatList[0];
	}

	for(vector<string>::const_iterator img1ID = this->params.genericParams.strList1.begin(),
		                               img2ID = this->params.genericParams.strList2.begin();
		img1ID != this->params.genericParams.strList1.end();
		img1ID++, img2ID++)
	{	
		printf("Computing fmatrix - (%s,%s) \n", img1ID->c_str(), img2ID->c_str());

		FeatureSet img1FeaSet, img2FeaSet;
		loadFeatureSet(*img1ID, img1FeaSet);
		loadFeatureSet(*img2ID, img2FeaSet);
		
		string motionFN = Disk::GetBaseFileName(*img1ID) + "-" + Disk::GetBaseFileName(*img2ID) + ".fm";
		Motion fmatrix;
		fmatrix.ReadFromFile(motionFN, &img1FeaSet, &img2FeaSet);
		ENSURE(fmatrix.type == MT_FUNDAMENTAL_MATRIX);
		PRINT_INT(fmatrix.matches.size());

		CFloatImage img1, img2;
		ImageIO::ReadFile(img1, *img1ID + CAMERA_ImageExt);
		ImageIO::ReadFile(img2, *img2ID + CAMERA_ImageExt);

		CFloatImage resultImg = FeatureRenderer::RenderEpipolarLineMatches(img1, img2, 
																		   img1FeaSet, img2FeaSet,
																		   fmatrix.matches, fmatrix.model,
																		   renderProb);

		string resultFN = outDir + OS_PATH_DELIMIT_STR + Disk::GetBaseFileName(*img1ID) + "-" + Disk::GetBaseFileName(*img2ID) + "-fm.tga";
		ImageIO::WriteFile(resultImg, resultFN);

		printf("\tDone\n\n");
	}
}