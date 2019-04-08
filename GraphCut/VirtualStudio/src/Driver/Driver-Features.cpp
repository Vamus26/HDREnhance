#include "Driver.h"
#include "Disk.h"
#include "Features.h"
#include "FeatureMatcher.h"
#include "FeatureRenderer.h"
#include "MotionFromMatches.h"

void Driver::combineFeatures()
{
	ENSURE(this->params.genericParams.strList1.size() > 0);
	ENSURE(this->params.genericParams.strList2.size() == 0);
	ENSURE(this->params.genericParams.intList1.size() == 0);
	ENSURE(this->params.genericParams.intList2.size() == 0);
	ENSURE(this->params.genericParams.floatList.size() == 0);

	for(vector<string>::const_iterator currImgID = this->params.genericParams.strList1.begin();
		currImgID != this->params.genericParams.strList1.end();
		currImgID++)
	{
		printf("Combining features - %s\n", currImgID->c_str());

		FeatureSet currImgFeaSet;
		loadFeatureSet(*currImgID, currImgFeaSet);
		PRINT_INT(currImgFeaSet.size());

		string combinedFeaSetFN = *currImgID + FEA_EXT; 
		ENSURE(Disk::DoesFileExist(combinedFeaSetFN));

		printf("\tDone\n\n");
	}	
}

void Driver::matchFeatures()
{
	ENSURE(this->params.genericParams.strList1.size() > 0);
	ENSURE(this->params.genericParams.strList1.size() ==
		   this->params.genericParams.strList2.size());
	ENSURE(this->params.genericParams.intList1.size() == 0);
	ENSURE(this->params.genericParams.intList2.size() == 0);
	ENSURE(this->params.genericParams.floatList.size() == 0);


	for(vector<string>::const_iterator img1ID = this->params.genericParams.strList1.begin(),
		                               img2ID = this->params.genericParams.strList2.begin();
		img1ID != this->params.genericParams.strList1.end();
		img1ID++, img2ID++)
	{	
		printf("Computing matches - (%s,%s) \n", img1ID->c_str(), img2ID->c_str());

		FeatureSet img1FeaSet, img2FeaSet;
		FeatureMatcher::FeatureMatcherParams feaMatcherParams;
		feaMatcherParams.loadFromFileIfAvaliable = false;
		feaMatcherParams.writeToFile = true;
		FeatureMatches feaMatches;
		loadFeatureMatches(*img1ID, *img2ID, 
						    img1FeaSet, img2FeaSet,
							feaMatches, feaMatcherParams);
		PRINT_INT(feaMatches.size());

		printf("\tDone\n\n");
	}
}

void Driver::computeFeaFM()
{
	ENSURE(this->params.genericParams.strList1.size() > 0);
	ENSURE(this->params.genericParams.strList1.size() ==
		   this->params.genericParams.strList2.size());
	ENSURE(this->params.genericParams.intList1.size() == 
		   this->params.genericParams.strList1.size());
	ENSURE(this->params.genericParams.intList1.size() == 
		   this->params.genericParams.intList2.size());

	ENSURE(this->params.genericParams.floatList.size() == 0);

	int iCurrIndex = 0;
	for(vector<string>::const_iterator img1ID = this->params.genericParams.strList1.begin(),
		                               img2ID = this->params.genericParams.strList2.begin();
		img1ID != this->params.genericParams.strList1.end();
		img1ID++, img2ID++, iCurrIndex++)
	{
		int img1Index = this->params.genericParams.intList1[iCurrIndex];
		int img2Index = this->params.genericParams.intList2[iCurrIndex];
		char noahMatchesFN[1024];
		sprintf(noahMatchesFN, "match-%03i-%03i.txt", img1Index, img2Index);

		string motionFN = Disk::GetBaseFileName(*img1ID) + "-" + Disk::GetBaseFileName(*img2ID) + ".fm";		

		string matchesFN = getMatchesFN(*img1ID, *img2ID);

		if((params.overwriteResults == false)  &&
			Disk::DoesFileExist(noahMatchesFN) &&
			Disk::DoesFileExist(motionFN)      &&
			Disk::DoesFileExist(matchesFN))
		{
			continue;
		}

		printf("Computing fmatrix - (%s,%s) \n", img1ID->c_str(), img2ID->c_str());

		FeatureSet img1FeaSet, img2FeaSet;
		FeatureMatches feaMatches;
		FeatureMatcher::FeatureMatcherParams feaMatcherParams = this->params.feaMatcherParams;
		feaMatcherParams.loadFromFileIfAvaliable = true;
		feaMatcherParams.writeToFile = true;
		loadFeatureMatches(*img1ID, *img2ID, 
						    img1FeaSet, img2FeaSet,
							feaMatches, feaMatcherParams);
		PRINT_INT(feaMatches.size());

		FeatureMatchSet motionInliers;
		MotionFromMatches mfm(img1FeaSet, img2FeaSet, feaMatches);
		Motion fmatrix = mfm.GetDominantMotion(motionInliers, MT_FUNDAMENTAL_MATRIX, this->params.mfmParams);		
		PRINT_INT(motionInliers.size());

		/*
		if(motionInliers.size() != 0)
		{
			printf("Refining matches and fmatrix\n");

			FeatureMatcher::FeatureMatcherParams feaMatcherParams = this->params.feaMatcherParams;
			feaMatcherParams.loadFromFileIfAvaliable = true;
			feaMatcherParams.writeToFile = true;
			feaMatcherParams.dataFileName = *img1ID + "-" + Disk::GetBaseFileName(*img2ID) + ".rmatches";
			
			FeatureMatcher feaMatcher(img1FeaSet, img2FeaSet);
			feaMatches.clear();
			feaMatcher.ComputeMatching(feaMatches, fmatrix, feaMatcherParams);
			PRINT_INT(feaMatches.size());

			motionInliers.clear();
			MotionFromMatches mfm(img1FeaSet, img2FeaSet, feaMatches);
			fmatrix = mfm.GetDominantMotion(motionInliers, MT_FUNDAMENTAL_MATRIX, this->params.mfmParams);
			PRINT_INT(motionInliers.size());
		}
		//*/
		
		fmatrix.WriteToFile(motionFN);

		fmatrix.matches.Write(noahMatchesFN);

		//fmatrix.model = CameraMats::CenterFMatrixCoordSys(fmatrix.model, 
		//												  ImageIO::GetImageShape(*img1ID + CAMERA_ImageExt),
		//												  ImageIO::GetImageShape(*img2ID + CAMERA_ImageExt));
		//fmatrix.matches.clear();
		//string motionFN_forMergeLite = Disk::GetBaseFileName(*img1ID) + "-" + Disk::GetBaseFileName(*img2ID) + ".cfm";
		//fmatrix.WriteToFile(motionFN_forMergeLite);

		printf("\tDone\n\n");
	}
}

void Driver::computeWarpHM()
{
	ENSURE(this->params.genericParams.strList1.size() > 0);
	ENSURE(this->params.genericParams.strList1.size() ==
		   this->params.genericParams.strList2.size());
	ENSURE(this->params.genericParams.intList1.size() == 
		   this->params.genericParams.strList1.size());
	ENSURE(this->params.genericParams.intList1.size() == 
		   this->params.genericParams.intList2.size());

	ENSURE(this->params.genericParams.floatList.size() == 0);

	int iCurrIndex = 0;
	for(vector<string>::const_iterator img1ID = this->params.genericParams.strList1.begin(),
		                               img2ID = this->params.genericParams.strList2.begin();
		img1ID != this->params.genericParams.strList1.end();
		img1ID++, img2ID++, iCurrIndex++)
	{
		int img1Index = this->params.genericParams.intList1[iCurrIndex];
		int img2Index = this->params.genericParams.intList2[iCurrIndex];
		char noahMatchesFN[1024];
		sprintf(noahMatchesFN, "match-%03i-%03i.txt", img1Index, img2Index);

		string motionFN = Disk::GetBaseFileName(*img1ID) + "-" + Disk::GetBaseFileName(*img2ID) + ".fm";		

		string matchesFN = getMatchesFN(*img1ID, *img2ID);

		if((params.overwriteResults == false)  &&
			Disk::DoesFileExist(noahMatchesFN) &&
			Disk::DoesFileExist(motionFN)      &&
			Disk::DoesFileExist(matchesFN))
		{
			continue;
		}

		printf("Computing fmatrix - (%s,%s) \n", img1ID->c_str(), img2ID->c_str());

		FeatureSet img1FeaSet, img2FeaSet;
		FeatureMatches feaMatches;
		FeatureMatcher::FeatureMatcherParams feaMatcherParams = this->params.feaMatcherParams;
		feaMatcherParams.loadFromFileIfAvaliable = true;
		feaMatcherParams.writeToFile = true;
		loadFeatureMatches(*img1ID, *img2ID, 
						    img1FeaSet, img2FeaSet,
							feaMatches, feaMatcherParams);
		PRINT_INT(feaMatches.size());

		FeatureMatchSet motionInliers;
		MotionFromMatches mfm(img1FeaSet, img2FeaSet, feaMatches);
		Motion hmMatrix = mfm.GetDominantMotion(motionInliers, MT_HOMOGRAPHY, this->params.mfmParams);		
		PRINT_INT(motionInliers.size());

		/*
		if(motionInliers.size() != 0)
		{
			printf("Refining matches and fmatrix\n");

			FeatureMatcher::FeatureMatcherParams feaMatcherParams = this->params.feaMatcherParams;
			feaMatcherParams.loadFromFileIfAvaliable = true;
			feaMatcherParams.writeToFile = true;
			feaMatcherParams.dataFileName = *img1ID + "-" + Disk::GetBaseFileName(*img2ID) + ".rmatches";
			
			FeatureMatcher feaMatcher(img1FeaSet, img2FeaSet);
			feaMatches.clear();
			feaMatcher.ComputeMatching(feaMatches, fmatrix, feaMatcherParams);
			PRINT_INT(feaMatches.size());

			motionInliers.clear();
			MotionFromMatches mfm(img1FeaSet, img2FeaSet, feaMatches);
			fmatrix = mfm.GetDominantMotion(motionInliers, MT_FUNDAMENTAL_MATRIX, this->params.mfmParams);
			PRINT_INT(motionInliers.size());
		}
		//*/
		
		hmMatrix.WriteToFile(motionFN);
		hmMatrix.matches.Write(noahMatchesFN);


		CFloatImage img1, img2;
		ImageIO::ReadFile(img1, *img1ID + ".tga");
		ImageIO::ReadFile(img2, *img2ID + ".tga");
		CFloatImage warpImg(img1.Shape());
		CByteImage maskImg(warpImg.Shape().width, warpImg.Shape().height, 1);
		ImageProcessing::TransformImage(warpImg, maskImg, hmMatrix.model, img2);
		string warpFN = Disk::GetBaseFileName(*img1ID) + "-" + Disk::GetBaseFileName(*img2ID) + ".tga";		
		ImageIO::WriteFile(warpImg, warpFN);

		//CFloatImage img1, img2;
		//ImageIO::ReadFile(img1, *img1ID + ".tga");
		//CFloatImage warpImg(img1.Shape());
		//CByteImage maskImg(warpImg.Shape().width, warpImg.Shape().height, 1);
		//for(int i = 0; i < 79; i++)
		//{
		//	char fn[1024];
		//	sprintf(fn, "in-%03i.tga", i);
		//	ImageIO::ReadFile(img2, fn);		
		//	ImageProcessing::TransformImage(warpImg, maskImg, hmMatrix.model, img2);
		//	sprintf(fn, "vid-%03i.tga", i);
		//	ImageIO::WriteFile(warpImg, fn);
		//}


		//fmatrix.model = CameraMats::CenterFMatrixCoordSys(fmatrix.model, 
		//												  ImageIO::GetImageShape(*img1ID + CAMERA_ImageExt),
		//												  ImageIO::GetImageShape(*img2ID + CAMERA_ImageExt));
		//fmatrix.matches.clear();
		//string motionFN_forMergeLite = Disk::GetBaseFileName(*img1ID) + "-" + Disk::GetBaseFileName(*img2ID) + ".cfm";
		//fmatrix.WriteToFile(motionFN_forMergeLite);

		printf("\tDone\n\n");
	}
}


void Driver::loadFeatureMatches(string img1ID, string img2ID, 
							    FeatureSet &img1FeaSet, FeatureSet &img2FeaSet,
							    FeatureMatches &feaMatches,
								FeatureMatcher::FeatureMatcherParams feaMatcherParams)
{
	loadFeatureSet(img1ID, img1FeaSet);
	loadFeatureSet(img2ID, img2FeaSet);

	feaMatcherParams.dataFileName = getMatchesFN(img1ID, img2ID);
	FeatureMatcher feaMatcher(img1FeaSet, img2FeaSet);
	feaMatcher.ComputeMatching(feaMatches, feaMatcherParams);
	if(feaMatcherParams.writeToFile)
	{
		ENSURE(Disk::DoesFileExist(feaMatcherParams.dataFileName));
	}
}

void Driver::loadFeatureSet(string imgID, FeatureSet &featureSet)
{	
	CShape imgShape = ImageIO::GetImageShape(imgID + CAMERA_ImageExt);
	featureSet.LoadAll(imgID, imgShape);
}
