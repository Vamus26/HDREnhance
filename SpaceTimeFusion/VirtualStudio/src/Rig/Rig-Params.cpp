#include "Rig.h"

Rig::RigParams Rig::RigParams::Default()
{
	RigParams params;
	params.frameSeq.startIndex = params.frameSeq.endIndex = -1;
	params.outDir = "." + OS_PATH_DELIMIT_STR;
	params.halfSizeCamMats = false;
	params.useGeoCamDists = false;
	params.targetCamIndex = -1;
	return params;
}

void Rig::RigParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sCapture rig params:\n", prefixPtr);

	printf("%s  halfSizeCamMats - %i\n", prefixPtr, halfSizeCamMats);
	printf("%s  useGeoCamDists - %i\n", prefixPtr, useGeoCamDists);

	ENSURE(cameraNames.size() > 0);
	printf("%s  Cameras in rig:\n", prefixPtr);
	for(uint iCamera = 0; iCamera < cameraNames.size(); iCamera++)
	{
		string camName = cameraNames[iCamera];
		//string camName = GetBaseDirName(cameraNames[iCamera]);
		printf("%s    %s\n", prefixPtr, camName.c_str());
	}

	//printf("\n%s  Camera neighs:\n", prefixPtr);
	//for(uint iNeigh = 0; iNeigh < camNeighPairs.size(); iNeigh++)
	//{		
	//	printf("%s    [%s-%s at dist - %f]\n",
	//			prefixPtr, 
	//			camNeighPairs[iNeigh].cam1ID.c_str(),
	//			camNeighPairs[iNeigh].cam2ID.c_str(),
	//			camNeighPairs[iNeigh].dist);
	//}

	printf("\n");

	if(frameSeq.startIndex != -1)
	{
		CHECK_FS_ORDER(frameSeq);
		printf("%s  [startFrameIndex, endFrameIndex] - [%i, %i]\n", 
			   prefixPtr, frameSeq.startIndex, frameSeq.endIndex);
	}

	int iTrack = 0;
	for(vector<Track>::const_iterator userDefinedTrack = userDefinedTracks.begin();
		userDefinedTrack != userDefinedTracks.end();
		userDefinedTrack++, iTrack++)
	{
		printf("%s  Track %i\n", prefixPtr, iTrack); 
		INSIST(userDefinedTrack->camIDs.size() >= 2);
		INSIST(userDefinedTrack->camIDs.size() == userDefinedTrack->points.size());

		int iCam = 0;
		for(vector<string>::const_iterator camID = userDefinedTrack->camIDs.begin();
			camID != userDefinedTrack->camIDs.end();
			camID++, iCam++)
		{
			printf("%s  \t %s [%lf, %lf]\n", prefixPtr, camID->c_str(), 
						userDefinedTrack->points[iCam][0], userDefinedTrack->points[iCam][1]);
		}
	}
	
	printf("%s  targetCamIndex - %i\n", prefixPtr, targetCamIndex);
	printf("%s  outDir         - %s\n", prefixPtr, outDir.c_str());

	printf("\n");
}
