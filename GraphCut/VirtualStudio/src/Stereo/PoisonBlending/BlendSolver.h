#ifndef __BLEND_SOLVER_H__
#define __BLEND_SOLVER_H__

#pragma warning(disable: 4512)

#include "Image.h"
#include "ImageProcessing.h"
#include "RegistrationDefs.h"
#include "SparseSolver.h"

const static float ConstraintWeight = 0.5f;

class BlendSolver
{
	typedef enum
	{
		NOT_VAR     = -2,
		CONSTRAINED = -1,
	} NodeType;

	typedef enum
	{
		SYSTEM_TYPE_IMG_COMPLETION,
		SYSTEM_TYPE_GRAD_BLEND
	} SystemType;

	const static bool SymmetricSolver = true; 

	CShape maskShape;
	CShape imgShape;

	CFloatImage      prevBlendImage;
	CImageOf<short>  labels;
	RegisteredPieces &pieces;

	int numOfVars;
	CImageOf<int>  maskToVarMap;
	CImageOf<uint> varToMaskMap;

	CFloatImage gradImg;
	CByteImage linkImgX;
	CByteImage linkImgY;

	RegisteredPiece holesPiece;	

public:
	typedef struct _BlendSolverParams
	{
		bool   ignoreHighGradLinks;
		float  highGradThres;
		bool   fillInHoles;
		bool   simpleHoleBlending;
		bool   useStrongBlending;
		int    holeBlendWidth;
		bool   blendGradients;
		ImageFilter holesFilter;
		double accuracy;
		int    maxIters;
		string debugDumpDir;
		static _BlendSolverParams Default()
		{
			_BlendSolverParams params;
			params.blendGradients      = true;
			params.fillInHoles         = true;
			params.accuracy            = 1e-7;
			params.maxIters            = 50; 
			params.highGradThres       = 0.5f;
			params.ignoreHighGradLinks = true;
			params.simpleHoleBlending  = false;
			params.useStrongBlending   = false;
			params.holeBlendWidth      = 11;
			params.holesFilter         = IF_LAB_COLOR_TRANSFER;
			//params.holesFilter         = IF_NO_OP;
			return params;
		}
		void Dump(const string prefix)
		{
			printf("%sBlend Solver Options:\n", prefix.c_str());
			if(fillInHoles == true)
			{				
				printf("%s  fillInHoles         = true\n",   prefix.c_str());
				switch(holesFilter)
				{
				case IF_LAB_COLOR_TRANSFER:
					printf("%s  holesFilter = LAB_COLOR_TRANSFER\n", prefix.c_str());
					break;
				case IF_NO_OP:
					printf("%s  holesFilter = NO_OP\n", prefix.c_str());
					break;
				default:
					REPORT_FAILURE("Unknown registration filter\n");
					break;
				}
			}
			else
			{
				printf("%s  fillInHoles         = false\n",   prefix.c_str());
			}
			if(ignoreHighGradLinks == true)
			{				
				printf("%s  ignoreHighGradLinks = true\n",   prefix.c_str());
				printf("%s  highGradThres       = %f\n",   prefix.c_str(), highGradThres);
			}
			else
			{
				printf("%s  ignoreHighGradLinks = false\n",   prefix.c_str());
			}

			if(useStrongBlending == true)
				printf("%s  useStrongBlending = true\n",   prefix.c_str());
			else
				printf("%s  useStrongBlending = false\n",   prefix.c_str());

			printf("%s  accuracy            = %lf\n",  prefix.c_str(), accuracy);
			printf("%s  maxIters            = %i\n",   prefix.c_str(), maxIters);			
			printf("%s  debugDumpDir        = %s\n", prefix.c_str(), debugDumpDir.c_str());
		}
	} BlendSolverParams;

	BlendSolver(CFloatImage blendImage,
				CImageOf<short> &labelImage, 
				RegisteredPieces &pieces);

	void Solve(CFloatImage &resultImg, CFloatImage queryImage, 
			   BlendSolverParams &params);

private:

	RegisteredPiece* getPiece(int label, bool fillInHoles)
	{
		if(label < 0)
		{
			if(fillInHoles == true)
			{
				return &(this->holesPiece);
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return &(this->pieces[label]);
		}
	}

	void simpleBlendInHoles( CFloatImage &blendImg, const BlendSolverParams params);


	void createBlendGradImages(int channel, bool fillInHoles, const BlendSolverParams &params);
		
	int createMaskVarMaps(bool fillInHoles, bool ignoreHighGradLinks);

	void solve(SystemType type, int channel, CFloatImage &resultImg, const BlendSolverParams params);

	void solve(SystemType type, CFloatImage &blendImg, const BlendSolverParams params);
    
	void setupSystem(SparseSolver &solver, int channel);

	uint pickRandomConstraintNode();

	void createLinkImages(bool fillInHoles, const BlendSolverParams params);
};

#endif
