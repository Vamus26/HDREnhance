#include "Rig.h"
#include "ScenePoints.h"
#include "Disk.h"
#include <algorithm>
#include "Draw.h"
#include "ImageIO.h"


void Rig::VisScenePlanes()
{
	string scenePtsFN = GetScenePtsFN();
	ENSURE(Disk::DoesFileExist(scenePtsFN));

	ScenePoints scenePoints;
	scenePoints.Read(scenePtsFN);

	for(uint iCam = 0; iCam < this->cameras.size(); iCam++)
	{
		if(this->cameras[iCam].frontoPlaneCount == 0)
		{
			INSIST(this->params.frameSeq.startIndex == this->params.frameSeq.endIndex);
			this->cameras[iCam].LoadPlanes(this->params.frameSeq.startIndex);
			INSIST(this->cameras[iCam].frontoPlaneCount > 0);
		}

		//ScenePoints camScenePoints = scenePoints;
		ScenePoints camScenePoints = scenePoints.GetVisibleScenePoints(&this->cameras[iCam]);
		camScenePoints.TransformCoordSys(this->cameras[iCam].cameraMats);		
		
		CVector3 pcLowerBound, pcUpperBound;
		camScenePoints.GetBounds(pcLowerBound, pcUpperBound);

		string camScenePointsFN = this->params.outDir + this->cameras[iCam].id + "-scenePts.ply";
		camScenePoints.WritePly(camScenePointsFN,
								vector<CVector3>(), false, 
								pcLowerBound, pcUpperBound, false,
								this->cameras[iCam].frontoPlanes, true);
	}
}

double Rig::GetMedianNeighVideoFrameDist()
{
	INSIST(this->cameras.size() > 0);
	vector<double> neighFrameDists;
	neighFrameDists.reserve(this->cameras.size());
	for(int iCam = 0; iCam < (int) this->cameras.size() - 1; iCam++)
	{
		if(IsVideoFrame(iCam) == false) continue;
		if(IsVideoFrame(iCam + 1) == false) continue;

		CVector3 nextCamCoord = this->cameras[iCam + 1].cameraMats.GetWorldCoordCoP();
		CVector3 currCamCoord = this->cameras[iCam].cameraMats.GetWorldCoordCoP();
		double neighCamDist = (nextCamCoord - currCamCoord).Length();
		neighFrameDists.push_back(neighCamDist);
	}
	INSIST(neighFrameDists.size() > 0);
	std::sort(neighFrameDists.begin(), neighFrameDists.end());
	double medianNeighCamDist = neighFrameDists[neighFrameDists.size()/2];

	return medianNeighCamDist;
}


CFloatImage Rig::GetFramePointVis(int camID, int iFrame, CVector2 pointLoc)
{
	CHECK_RANGE(camID, 0, (int) this->cameras.size() - 1);
	string frameName = this->cameras[camID].GetFrameName(iFrame);

	CFloatImage frameImg;
	ImageIO::ReadFile(frameImg, frameName);
	INSIST(frameImg.Shape().InBounds(pointLoc[0], pointLoc[1]));

	Draw::DrawPoint(frameImg, NEAREST_INT(pointLoc[0]), NEAREST_INT(pointLoc[1]), 2, CVector3(1.0, 0.0, 0.0), true);

	return frameImg;
}


bool Rig::IsCamCoveredByTrack(int camIndex, const Track &track)
{
	INSIST(track.camIDs.size() >= 2);

	string firstCamID = track.camIDs[0];
	int firstCamIndex;
	Camera *firstCam = GetCamera(firstCamID, firstCamIndex);
	INSIST(IsVideoFrame(firstCamIndex) == true);
	
	string lastCamID = track.camIDs[track.camIDs.size() - 1];
	int lastCamIndex;
	Camera *lastCam = GetCamera(lastCamID, lastCamIndex);
	INSIST(IsVideoFrame(lastCamIndex) == true);

	INSIST(firstCamIndex <= lastCamIndex)
	if(InRange(camIndex, firstCamIndex, lastCamIndex))
		return true;
	else
		return false;
}

CVector2 Rig::GetTrackPoint(int camIndex, const Track &track)
{
	INSIST(track.camIDs.size() >= 2);

	for(uint iCam = 1; iCam < track.camIDs.size(); iCam++)
	{
		string currCamID = track.camIDs[iCam];
		int currCamIndex;
		Camera *currCam = GetCamera(currCamID, currCamIndex);
		INSIST(IsVideoFrame(currCamIndex) == true);
		
		string prevCamID = track.camIDs[iCam - 1];
		int prevCamIndex;
		Camera *prevCam = GetCamera(prevCamID, prevCamIndex);
		INSIST(IsVideoFrame(prevCamID) == true);

		INSIST(prevCamIndex <= currCamIndex);
		if(InRange(camIndex, prevCamIndex, currCamIndex))
		{
			CVector2 endLoc   = track.points[iCam];
			CVector2 startLoc = track.points[iCam - 1];
			CVector2 trackPointLoc;

			if(camIndex == prevCamIndex)
				trackPointLoc = startLoc;
			else
			{
				double camIndexFrac = ((double) (camIndex - prevCamIndex)) / (currCamIndex - prevCamIndex);
				trackPointLoc = ((endLoc - startLoc) * camIndexFrac) + startLoc;
			}

			return trackPointLoc;
		}		
	}

	REPORT_FAILURE("Camera - %s not covered by track\n", this->cameras[camIndex].id.c_str());
}



void Rig::VisTracks()
{
	INSIST(this->params.userDefinedTracks.size() > 0);

	int iTrack = 0;
	for(vector<Track>::const_iterator userDefinedTrack = this->params.userDefinedTracks.begin();
		userDefinedTrack != this->params.userDefinedTracks.end();
		userDefinedTrack++, iTrack++)
	{
		printf("Track %i\n", iTrack);
		INSIST(userDefinedTrack->camIDs.size() >= 2);
		INSIST(userDefinedTrack->camIDs.size() == userDefinedTrack->points.size());

		for(uint iCam = 1; iCam < userDefinedTrack->camIDs.size(); iCam++)
		{
			string currCamID = userDefinedTrack->camIDs[iCam];
			int currCamIndex;
			Camera *currCam = GetCamera(currCamID, currCamIndex);
			INSIST(IsVideoFrame(currCamIndex) == true);
			CVector2 endLoc = userDefinedTrack->points[iCam];

			string prevCamID = userDefinedTrack->camIDs[iCam - 1];
			int prevCamIndex;
			Camera *prevCam = GetCamera(prevCamID, prevCamIndex);
			INSIST(IsVideoFrame(prevCamID) == true);
			CVector2 startLoc = userDefinedTrack->points[iCam - 1];

			INSIST(prevCamIndex <= currCamIndex);

			for(int camIndex = prevCamIndex; camIndex <= currCamIndex; camIndex++)
			{
				printf("."); fflush(stdout);

				CVector2 currLoc = GetTrackPoint(camIndex, *userDefinedTrack);

				CFloatImage framePointVis = GetFramePointVis(camIndex, 0, currLoc);
				char fileName[1024];
				sprintf(fileName, "%strack-%03i-%s.tga", 
						this->params.outDir.c_str(), iTrack, this->cameras[camIndex].id.c_str());
				ImageIO::WriteFile(framePointVis, fileName);
			}
		}
		printf("\n");
	}
}