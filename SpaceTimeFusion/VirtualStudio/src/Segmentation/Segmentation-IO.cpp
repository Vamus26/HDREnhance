#include "Segmentation.h"
#include "Disk.h"
#include "ImageIO.h"

bool Segmentation::Load(const Camera &camera, int iFrame, bool loadBeliefDataOnly)
{
	ENSURE(this->segments.size() == 0);

	bool loadSuccess = false;	
	
	string frameSmoothName = camera.GetFrameSmoothName(iFrame);
	string frameLabelName  = camera.GetFrameLabelName(iFrame);
	if(Disk::DoesFileExist(frameSmoothName) &&
	   Disk::DoesFileExist(frameLabelName))
	{
		CShape imgShape = camera.GetFrameShape(iFrame);

		ImageIO::ReadFile(this->smoothImg, frameSmoothName);
		ENSURE(this->smoothImg.Shape() == imgShape);

		ImageIO::ReadRaw(this->labelImg, frameLabelName);		
		ENSURE(imgShape.SameIgnoringNBands(this->labelImg.Shape()));
		ENSURE(this->labelImg.Shape().nBands == 1);

		string segFN     = camera.GetFrameSegName(iFrame);
		string beliefsFN = camera.GetFramePrevBeliefsName(iFrame);
		string msgesFN   = camera.GetFramePrevMsgesName(iFrame);
		if((Disk::DoesFileExist(segFN)     == true)  &&
		   (Disk::DoesFileExist(beliefsFN) == true)  &&
		   (Disk::DoesFileExist(msgesFN)   == true))
		{
			if(loadBeliefDataOnly == false)
			{
				loadSegmentsStaticData(camera, iFrame);
				ComputeSpatialNeighs();
				loadSegmentsMessages(camera, iFrame);
			}
			else
			{
				CreateSegments();
			}

			loadSegmentsBeliefs(camera, iFrame);

			this->fullyComputed = true;
		}
		else
		{
			Disk::DeleteFile(segFN);
			Disk::DeleteFile(beliefsFN);
			Disk::DeleteFile(msgesFN);

			CreateSegments();

			this->fullyComputed = false;
		}

		loadSuccess = true;
	}	

	return loadSuccess;
}

//check - change file name save to saveSpecific
void Segmentation::Save(const Camera &camera, int iFrame)
{		
	string frameSmoothName = camera.GetFrameSmoothName(iFrame);
	ImageIO::WriteFile(this->smoothImg, frameSmoothName);
	//ENSURE(CAMERA_ImageExt.compare(".tif") == 0);
	//ImageIO::WriteFileTiff(this->smoothImg, frameSmoothName, CAMERA_ImageCompression);

	string frameLabelName = camera.GetFrameLabelName(iFrame);
	ImageIO::WriteRaw(this->labelImg, frameLabelName);

	string frameLabelEdgesName = camera.GetFrameLabelEdgesName(iFrame);
	CByteImage labelsEdgesImg  = this->GetBoundaryImg();
	ImageIO::WriteFile(labelsEdgesImg, frameLabelEdgesName);
}

int Segmentation::getStaticDataFileSize()
{
	int fileSize = 4 * sizeof(int); //seg count, view count, plane count, sceneplane count

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		fileSize += currSeg->GetStaticDataFileSize();
	}

	return fileSize;
}

void Segmentation::serializeStaticData(char **memBuffPtr)
{
	char *memBuffCopy = *memBuffPtr;

	ENSURE(this->segments.size() >  0);
	ENSURE(this->viewCount       >  0);
	ENSURE(this->planeCount      >  0);
	ENSURE(this->scenePlaneCount >= 0);

	WRITE_INT(memBuffPtr, (int) this->segments.size());
	WRITE_INT(memBuffPtr, this->viewCount);
	WRITE_INT(memBuffPtr, this->planeCount);
	WRITE_INT(memBuffPtr, this->scenePlaneCount);

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		currSeg->SerializeStaticData(memBuffPtr);
	}

	ENSURE((*memBuffPtr - memBuffCopy) == getStaticDataFileSize());
}

void Segmentation::unserializeStaticData(char **memBuffPtr)
{
	int segCount;
	READ_INT(memBuffPtr, segCount);
	READ_INT(memBuffPtr, this->viewCount);
	READ_INT(memBuffPtr, this->planeCount);
	READ_INT(memBuffPtr, this->scenePlaneCount);

	ENSURE(segCount > 0);
	ENSURE(this->viewCount > 0);
	ENSURE(this->planeCount > 0);
	ENSURE(this->scenePlaneCount >= 0);

	ENSURE(this->segments.size() == 0);
	this->segments.resize(segCount);
	AllocSegmentBuffers(this->viewCount, this->planeCount, this->scenePlaneCount);	

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		currSeg->UnserializeStaticData(memBuffPtr);
	}
}

void Segmentation::SaveSegmentsStaticData(const Camera &camera, int iFrame)
{
	int memSize = getStaticDataFileSize();
	char *memBuff = new char[memSize];
	ENSURE(memBuff != NULL);
	char *memBuffPtr = memBuff;

	serializeStaticData(&memBuffPtr);
	ENSURE((memBuffPtr - memBuff) == memSize);

	string segFN = camera.GetFrameSegName(iFrame);
	Disk::SaveBinFile(memBuff, memSize, segFN);

	delete [] memBuff;
}


void Segmentation::loadSegmentsStaticData(const Camera &camera, int iFrame)
{
	string segFN = camera.GetFrameSegName(iFrame);

	int memSize = 0;
	char *memBuff = Disk::LoadBinFile(segFN, memSize);	
	ENSURE(memBuff != NULL);
	char *memBuffPtr = memBuff;
	
	unserializeStaticData(&memBuffPtr);
	ENSURE((memBuffPtr - memBuff) == memSize);	

	delete [] memBuff;
}

void Segmentation::SaveSegmentsDynamicData(const Camera &camera, int iFrame)
{
	saveSegmentsBeliefs(camera, iFrame);
	saveSegmentsMessages(camera, iFrame);
}

void Segmentation::saveSegmentsBeliefs(const Camera &camera, int iFrame)
{
	int memSize = 0;
	char *memBuff = getBeliefsSerialization(memSize);
	ENSURE(memBuff != NULL);

	string beliefFN = camera.GetFrameBeliefsName(iFrame);
	Disk::SaveBinFile(memBuff, memSize, beliefFN);

	delete [] memBuff;
}

void Segmentation::loadSegmentsBeliefs(const Camera &camera, int iFrame)
{
	string beliefFN = camera.GetFramePrevBeliefsName(iFrame);

	int memSize = 0;
	char *memBuff = Disk::LoadBinFile(beliefFN, memSize);
	ENSURE(memBuff != NULL);
	
	char *memBuffPtr = memBuff;
	unserializeBeliefs(&memBuffPtr);
	ENSURE((memBuffPtr - memBuff) == memSize);

	delete [] memBuff;
}

char* Segmentation::getBeliefsSerialization(int &memSize)
{
	ENSURE(this->segments.size() > 0);
	ENSURE(this->viewCount       > 0);
	ENSURE(this->planeCount      > 0);
	ENSURE(this->scenePlaneCount >= 0);
	
	memSize = 4 * sizeof(int); //seg count, view count, planeCount, scenePlaneCount
	memSize += (int) this->segments.size() * 
			   this->segments[0].segData->GetBeliefFileSize();			   

	char *memBuff = new char[memSize];
	ENSURE(memBuff != NULL);
	char *memBuffPtr = memBuff;

	WRITE_INT(&memBuffPtr, (int) this->segments.size());
	WRITE_INT(&memBuffPtr, this->viewCount);
	WRITE_INT(&memBuffPtr, this->planeCount);
	WRITE_INT(&memBuffPtr, this->scenePlaneCount);

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		currSeg->segData->SerializeBelief(&memBuffPtr);		
	}

	ENSURE((memBuffPtr - memBuff) == memSize);

	return memBuff;
}


void Segmentation::unserializeBeliefs(char **memBuffPtr)
{
	int segCount = 0;
	int vCount   = 0;
	int pCount   = 0;
	int sCount   = 0;	

	READ_INT(memBuffPtr, segCount);
	READ_INT(memBuffPtr, vCount);
	READ_INT(memBuffPtr, pCount);
	READ_INT(memBuffPtr, sCount);
	
	ENSURE(segCount == (int) this->segments.size());
	if(this->planeCount == 0)
	{
		this->viewCount       = vCount;
		this->planeCount      = pCount;		
		this->scenePlaneCount = sCount;
	}
	else
	{		
		ENSURE(vCount   == this->viewCount);
		ENSURE(pCount   == this->planeCount);
		ENSURE(sCount   == this->scenePlaneCount);
	}

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		if(currSeg->segData == NULL)
			currSeg->segData = new SegmentData(this->viewCount, this->planeCount, this->scenePlaneCount, false);
		currSeg->segData->UnserializeBelief(memBuffPtr);
	}
}

void Segmentation::saveSegmentsMessages(const Camera &camera, int iFrame)
{
	int memSize = 0;
	char *memBuff = getMsgesSerialization(memSize);
	ENSURE(memBuff != NULL);

	string msgesFN = camera.GetFrameMsgesName(iFrame);
	Disk::SaveBinFile(memBuff, memSize, msgesFN);

	delete [] memBuff;
}

void Segmentation::loadSegmentsMessages(const Camera &camera, int iFrame)
{
	string msgesFN = camera.GetFramePrevMsgesName(iFrame);
	int memSize = 0;
	char *memBuff = Disk::LoadBinFile(msgesFN, memSize);
	ENSURE(memBuff != NULL);
	
	char *memBuffPtr = memBuff;
	unserializeMsges(&memBuffPtr);
	ENSURE((memBuffPtr - memBuff) == memSize);

	delete [] memBuff;
}

char* Segmentation::getMsgesSerialization(int &memSize)
{
	ENSURE(this->segments.size() >  0);
	ENSURE(this->viewCount       >  0);
	ENSURE(this->planeCount      >  0);
	ENSURE(this->scenePlaneCount >= 0);
	
	memSize = 4 * sizeof(int); //seg count, view count, planeCount, scenePlaneCount

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		memSize += currSeg->spatialNeighs.GetMessagesFileSize();
	}

	char *memBuff = new char[memSize];
	ENSURE(memBuff != NULL);
	char *memBuffPtr = memBuff;

	WRITE_INT(&memBuffPtr, (int) this->segments.size());
	WRITE_INT(&memBuffPtr, this->viewCount);
	WRITE_INT(&memBuffPtr, this->planeCount);
	WRITE_INT(&memBuffPtr, this->scenePlaneCount);

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		currSeg->spatialNeighs.SerializeMessages(&memBuffPtr);		
	}

	ENSURE((memBuffPtr - memBuff) == memSize);

	return memBuff;
}

void Segmentation::unserializeMsges(char **memBuffPtr)
{
	int segCount = 0;
	int vCount   = 0;
	int pCount   = 0;
	int sCount   = 0;	

	READ_INT(memBuffPtr, segCount);
	READ_INT(memBuffPtr, vCount);
	READ_INT(memBuffPtr, pCount);
	READ_INT(memBuffPtr, sCount);
	
	ENSURE(segCount == (int) this->segments.size());
	ENSURE(pCount   == this->planeCount);
	ENSURE(vCount   == this->viewCount);
	ENSURE(sCount   == this->scenePlaneCount);

	for(vector<Segment>::iterator currSeg = this->segments.begin();
		currSeg != this->segments.end();
		currSeg++)
	{
		currSeg->spatialNeighs.UnserializeMessages(memBuffPtr);
	}
}

bool Segmentation::CurrIterDynamicDataExists(const Camera &camera, int iFrame)
{
	string newMsgFN    = camera.GetFrameMsgesName(iFrame);
	string newBeliefFN = camera.GetFrameBeliefsName(iFrame);

	//bug
	if(camera.cameraNeighs.size() == 0) return true;

	if(Disk::DoesFileExist(newMsgFN) &&	Disk::DoesFileExist(newBeliefFN))
		return true;
	else
		return false;
}

void Segmentation::UpdatePrevIterDynamicData(const Camera &camera, int iFrame)
{	
	//bug
	if(camera.cameraNeighs.size() == 0) return;

	string newMsgFN = camera.GetFrameMsgesName(iFrame);
	string prevMsgFN = camera.GetFramePrevMsgesName(iFrame);	
	Disk::RenameFile(newMsgFN, prevMsgFN, true);

	string newBeliefFN  = camera.GetFrameBeliefsName(iFrame);
	string prevBeliefFN = camera.GetFramePrevBeliefsName(iFrame);	
	Disk::RenameFile(newBeliefFN, prevBeliefFN, true);
}
