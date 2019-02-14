#include "Camera.h"
#include "Disk.h"

string Camera::getFramesPattern()
{
	return (this->path + CAMERA_ColorDirName + this->id + "-" + CAMERA_FramePrefix + "?????" + CAMERA_ImageExt);
}

string Camera::GetTimeStampsFN() const
{
	return (this->path + this->id + "-" + CAMERA_TimeStampsFN);
}

string Camera::GetCameraMatsFN() const
{
	return (this->path + this->id + "-" + CAMERA_CameraMatsFN);
}

string Camera::GetCameraViewPtsFN() const
{
	return (this->path + this->id + "-" + CAMERA_CameraViewPtsFN);
}

string Camera::GetFrontoPlanesFN() const
{
	return (this->path + this->id + "-" + CAMERA_CameraFrontoPlanesFN);
}

string Camera::GetScenePlanesFN(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = this->path + this->id;	

	sprintf(fileBuf, "%s-%05i-%s", 
			filePrefix.c_str(), 
			iFrame, 
			CAMERA_CameraScenePlanesFN.c_str());
	string scenePlaneFN = fileBuf;

	return scenePlaneFN;
}

string Camera::GetFrameName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			CAMERA_ImageExt.c_str());
	string frameName = fileBuf;

	return frameName;
}


string Camera::GetFrameReconMapName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(), 
			CAMERA_ReconPrefix.c_str(),
			iFrame, 
			CAMERA_RawExt.c_str());
	string frameName = fileBuf;

	return frameName;
}

string Camera::GetFrameAnalogyName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s", 	
			filePrefix.c_str(),
			CAMERA_FramePrefix.c_str(),
			iFrame,
			CAMERA_AnalogySuffix.c_str(),
			CAMERA_ImageExt.c_str());
	string frameName = fileBuf;

	return frameName;
}

string Camera::GetFrameAnalogyMaskName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s%s", 	
			filePrefix.c_str(),
			CAMERA_FramePrefix.c_str(),
			iFrame,
			CAMERA_AnalogySuffix.c_str(),
			CAMERA_MaskSuffix.c_str(),
			CAMERA_ImageExt.c_str());
	string frameName = fileBuf;

	return frameName;
}

string Camera::GetFrameNameLarge(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			CAMERA_LargeSuffix.c_str(),
			CAMERA_ImageExt.c_str());
	string frameName = fileBuf;

	return frameName;
}

string Camera::GetFrameAnalogyNameLarge(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s%s", 
			filePrefix.c_str(),
			CAMERA_FramePrefix.c_str(),
			iFrame,			
			CAMERA_AnalogySuffix.c_str(),
			CAMERA_LargeSuffix.c_str(),
			CAMERA_ImageExt.c_str());
	string frameName = fileBuf;

	return frameName;
}

string Camera::GetFrameNameCrop(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			CAMERA_CropSuffix.c_str(),
			CAMERA_ImageExt.c_str());
	string frameName = fileBuf;

	return frameName;
}

string Camera::GetFrameFlowName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			".flow");
	string frameFlowName = fileBuf;

	return frameFlowName;
}

string Camera::GetFrameRevFlowName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			".revflow");
	string frameRevFlowName = fileBuf;

	return frameRevFlowName;
}


string Camera::GetFrameFlowNameCrop(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			CAMERA_CropSuffix.c_str(),
			".flow");
	string frameFlowName = fileBuf;

	return frameFlowName;
}

string Camera::GetFrameRevFlowNameCrop(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			CAMERA_CropSuffix.c_str(),
			".revflow");
	string frameRevFlowName = fileBuf;

	return frameRevFlowName;
}


string Camera::GetFrameFlowNameLarge(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			CAMERA_LargeSuffix.c_str(),
			".flow");
	string frameFlowName = fileBuf;

	return frameFlowName;
}

string Camera::GetFrameRevFlowNameLarge(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetFramesDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s%s", 
			filePrefix.c_str(), 
			CAMERA_FramePrefix.c_str(),
			iFrame, 
			CAMERA_LargeSuffix.c_str(),
			".revflow");
	string frameRevFlowName = fileBuf;

	return frameRevFlowName;
}

string Camera::GetFrameSmoothName(int index) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegDirName() + this->id + "-";	

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FrameSmoothPrefix.c_str(),
			index, 
			CAMERA_ImageExt.c_str());
	string frameSmoothName = fileBuf;

	return frameSmoothName;
}

string Camera::GetFrameSegName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FrameSegPrefix.c_str(),
			iFrame, 
			CAMERA_RawExt.c_str());
	string frameSegName = fileBuf;

	return frameSegName;
}

string Camera::GetFrameBeliefsName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FrameBeliefPrefix.c_str(),
			iFrame, 
			CAMERA_RawExt.c_str());
	string frameBeliefsName = fileBuf;

	return frameBeliefsName;
}

string Camera::GetFrameMsgesName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FrameMsgesPrefix.c_str(),
			iFrame, 
			CAMERA_RawExt.c_str());
	string frameMsgesName = fileBuf;

	return frameMsgesName;
}

string Camera::GetFramePrevBeliefsName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FramePrevBeliefPrefix.c_str(),
			iFrame, 
			CAMERA_RawExt.c_str());
	string frameBeliefsName = fileBuf;

	return frameBeliefsName;
}

string Camera::GetFramePrevMsgesName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FramePrevMsgesPrefix.c_str(),
			iFrame, 
			CAMERA_RawExt.c_str());
	string frameMsgesName = fileBuf;

	return frameMsgesName;
}


string Camera::GetFrameBakPrevBeliefsName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegBakDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FramePrevBeliefPrefix.c_str(),
			iFrame, 
			CAMERA_RawExt.c_str());
	string frameBeliefsName = fileBuf;

	return frameBeliefsName;
}

string Camera::GetFrameBakPrevMsgesName(int iFrame) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegBakDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FramePrevMsgesPrefix.c_str(),
			iFrame, 
			CAMERA_RawExt.c_str());
	string frameMsgesName = fileBuf;

	return frameMsgesName;
}


string Camera::GetFrameLabelName(int index) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FrameLabelPrefix.c_str(),
			index, 
			CAMERA_RawExt.c_str());
	string frameLabelName = fileBuf;

	return frameLabelName;
}

string Camera::GetFrameLabelEdgesName(int index) const
{
	static char fileBuf[1024];
	string filePrefix = GetSegDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FrameLabelEdgesPrefix.c_str(),
			index, 
			CAMERA_ImageExt.c_str());
	string frameLabelName = fileBuf;

	return frameLabelName;
}


string Camera::GetFrameDepsVisName(int index) const
{
	static char fileBuf[1024];
	string filePrefix = GetDepsDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FrameDepthsPrefix.c_str(),
			index, 
			CAMERA_ImageExt.c_str());
	string frameDepsVisName = fileBuf;

	return frameDepsVisName;
}

string Camera::GetFrameDepsName(int index) const
{
	static char fileBuf[1024];
	string filePrefix = GetDepsDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s", 
			filePrefix.c_str(),
			CAMERA_FrameDepthsPrefix.c_str(),
			index, 
			CAMERA_RawExt.c_str());
	string frameDepsName = fileBuf;

	return frameDepsName;
}

string Camera::GetFrameDepsNameAlt(int index) const
{
	static char fileBuf[1024];
	string filePrefix = GetDepsDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i%s%s", 
			filePrefix.c_str(),
			CAMERA_FrameDepthsPrefix.c_str(),
			index,
			CAMERA_AltSuffix.c_str(),
			CAMERA_RawExt.c_str());
	string frameDepsName = fileBuf;

	return frameDepsName;
}

string Camera::GetFrameFlowDataCostFN(int iFrame, int offsetX, int offsetY) const
{
	static char fileBuf[1024];
	string filePrefix = GetFlowDirName() + this->id + "-";

	sprintf(fileBuf, "%s%s%05i_%+04i_%+04i%s", 
			filePrefix.c_str(),
			CAMERA_FrameFlowDC_Prefix.c_str(),
			iFrame, 
			offsetX,
			offsetY,
			CAMERA_RawExt.c_str());
	string frameFlowDC_name = fileBuf;

	return frameFlowDC_name;
}

string Camera::GetFramesDirName() const
{
	return (this->path + CAMERA_ColorDirName);
}

string Camera::GetSegDirName() const
{
	return (this->path + CAMERA_SegDirName);
}

string Camera::GetSegBakDirName() const
{
	return this->path + CAMERA_SegDirName + "bak" + OS_PATH_DELIMIT_STR;
}

string Camera::GetFlowDirName() const
{
	return (this->path + CAMERA_FlowDirName);
}

string Camera::GetDepsDirName() const
{
	return (this->path + CAMERA_DepthsDirName);
}

