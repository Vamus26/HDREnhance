#ifndef __SCENE_SEGMENTER_H__
#define __SCENE_SEGMENTER_H__

#include "Definitions.h"
#include "PointCloud.h"
#include "PlaneFromPoints.h"
#include <fstream>

class PCloudSegmenter
{
	PointCloud scenePoints;

public:
	typedef struct _PCloudSegmenterParams
	{
		PlaneFromPoints::PlaneFromPointsParams ransacParams;		
		float ransacThresMult;
		uint maxPlanesInScene;
		bool createPlyFile;
		string plyFN;

		static _PCloudSegmenterParams Default()
		{
			_PCloudSegmenterParams params;
			params.ransacThresMult  = 10.0f;
			params.maxPlanesInScene = 0;
			params.createPlyFile    = true;
			params.plyFN            = "sceneSeg.ply";
			params.ransacParams     = PlaneFromPoints::PlaneFromPointsParams::Default();
			return params;
		}

		void Dump(string prefix)
		{
			printf("%sPoint cloud segmenter options:\n", prefix.c_str());		
			
			ransacParams.Dump("  ");

			if(!createPlyFile)
				printf("%s  createPlyFile: false\n", prefix.c_str());
			else
			{
				printf("%s  createPlyFile: true\n", prefix.c_str());
				printf("%s  plyFN:         %s\n",   prefix.c_str(), plyFN.c_str());
			}

			printf("%s   ransacThresMult: %f\n", prefix.c_str(), ransacThresMult);
			printf("%s  maxPlanesInScene: %i\n", prefix.c_str(), maxPlanesInScene);				
			
			printf("\n");
		}
	} PCloudSegmenterParams;

	PCloudSegmenter(const PointCloud &points);

	void SegmentPointCloud(vector<Plane> &planes, PCloudSegmenterParams params = PCloudSegmenterParams::Default());
	

private:
	void writePlyFile(string fileName, const vector<Plane> &planes, int visPlaneIndex = -1);
	void writePlyFile(ofstream &outStream, const vector<Plane> &planes, int visPlaneIndex = -1);
};

#endif
