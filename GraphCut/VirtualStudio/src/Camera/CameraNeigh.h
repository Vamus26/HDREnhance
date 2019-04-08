#ifndef __CAMERA_NEIGH_H__
#define __CAMERA_NEIGH_H__

#include "Definitions.h"

typedef struct _CameraNeigh
{
	string id;
	float dist;

	_CameraNeigh()
	{
		this->id   = "";
		this->dist = 0.0f;
	}
} CameraNeigh;

class CameraNeighs : public vector<CameraNeigh>
{
public:
	void Load(string filename)
	{
		ifstream instream;
		instream.open(filename.c_str());
		ENSURE(instream.is_open());

		string lineBuff;
		while(std::getline(instream, lineBuff))
		{
			vector<string> lineTokens;	
			Tokenize(lineBuff, lineTokens, " \t");

			if(lineTokens.size() == 0) 
				continue;

			ENSURE(lineTokens.size() == 2);
		
			CameraNeigh newNeigh;
			newNeigh.id = lineTokens[0];
			newNeigh.dist = (float) atof(lineTokens[1].c_str());
			ENSURE(newNeigh.id.size() > 0);
			ENSURE(newNeigh.dist > 0.0f);
			this->push_back(newNeigh);
		}

		instream.close();
	}
};



#endif
