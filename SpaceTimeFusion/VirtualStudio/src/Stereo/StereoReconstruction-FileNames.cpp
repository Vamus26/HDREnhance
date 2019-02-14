#include "StereoReconstruction.h"
#include "Disk.h"

const string STEREO_RECONSTRUCTION_RegPrefix        = "reg-";
const string STEREO_RECONSTRUCTION_BlendPrefix      = "blend-";
const string STEREO_RECONSTRUCTION_MaskPrefix       = "mask-";
const string STEREO_RECONSTRUCTION_NovelViewPrefix  = "nv-";

string StereoReconstruction::getTargetContextRegFN()
{
	int iFrame = this->srParams.iFrame;	
	Camera *targetCam = &this->targetContextSRS.sourceCam;
	return GetTargetContextRegFN(targetCam, iFrame, this->rig.params.outDir);	
}

string StereoReconstruction::getTargetContextBlendFN()
{
	int iFrame = this->srParams.iFrame;	
	Camera *targetCam = &this->targetContextSRS.sourceCam;
	return GetTargetContextBlendFN(targetCam, iFrame, this->rig.params.outDir);	
}

string StereoReconstruction::getTargetContextMaskFN()
{
	int iFrame = this->srParams.iFrame;	
	Camera *targetCam = &this->targetContextSRS.sourceCam;
	return GetTargetContextMaskFN(targetCam, iFrame, this->rig.params.outDir);
}

string StereoReconstruction::getTargetContextNovelViewFN()
{
	int iFrame = this->srParams.iFrame;	
	Camera *targetCam = &this->targetContextSRS.sourceCam;
	return GetTargetContextNovelViewFN(targetCam, iFrame, this->rig.params.outDir);	
}

string StereoReconstruction::GetTargetContextRegFN(const Camera *cam, int iFrame, string outDir)
{
	string resultFN = outDir + STEREO_RECONSTRUCTION_RegPrefix + 
	                  Disk::GetBaseFileName(cam->GetFrameName(iFrame));	
	return resultFN;
}

string StereoReconstruction::GetTargetContextBlendFN(const Camera *cam, int iFrame, string outDir)
{
	string resultFN = outDir + STEREO_RECONSTRUCTION_BlendPrefix + 
	                  Disk::GetBaseFileName(cam->GetFrameName(iFrame));	
	return resultFN;
}

string StereoReconstruction::GetTargetContextMaskFN(const Camera *cam, int iFrame, string outDir)
{
	string resultFN = outDir + STEREO_RECONSTRUCTION_MaskPrefix + 
	                  Disk::GetBaseFileName(cam->GetFrameName(iFrame));	
	return resultFN;
}


string StereoReconstruction::GetTargetContextNovelViewFN(const Camera *cam, int iFrame, string outDir)
{
	string resultFN = outDir + STEREO_RECONSTRUCTION_NovelViewPrefix + 
	                  Disk::GetBaseFileName(cam->GetFrameName(iFrame));	
	return resultFN;
}



