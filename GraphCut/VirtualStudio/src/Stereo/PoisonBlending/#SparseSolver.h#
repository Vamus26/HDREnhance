#ifndef __SPARSE_SOLVER_H__
#define __SPARSE_SOLVER_H__

#include "Definitions.h"
#include "laspack/qmatrix.h"
#include "laspack/errhandl.h"
#include "laspack/itersolv.h"
#include "laspack/rtc.h"
#include "Debug.h"

class SparseSolver
{  
  int  varCount;

  QMatrix aMat;
  QMatrix *aMatPtr;

  Vector  xVec;
  Vector  *xVecPtr;

  Vector  bVec;
  Vector  *bVecPtr;

  bool symmetric;

private:
	void errCheck()
	{
		LASErrIdType lasERR = LASResult();
		if(lasERR != LASOK)
		{
			WriteLASErrDescr(stdout);
			REPORT_FAILURE("LASPACK failure id: %i", lasERR);
		}
	}

public:

	void SetRow(int iRow, int elemCount, double rowX, double rowB, int *elemIDs, double *elemCoefs) 
	{
		ENSURE(iRow >= 0);
		ENSURE(iRow < this->varCount);

		Q_SetLen(this->aMatPtr, iRow + 1, elemCount);
		V_SetCmp(this->xVecPtr, iRow + 1, rowX);
		V_SetCmp(this->bVecPtr, iRow + 1, rowB);
		for(int iElem = 0; iElem < elemCount; iElem++)
		{
			ENSURE(elemIDs[iElem] >= 0);
			ENSURE(elemIDs[iElem] < this->varCount);
			if(this->symmetric == true) 
			{
				ENSURE(elemIDs[iElem] >= iRow);
			}
			Q_SetEntry(this->aMatPtr, iRow + 1, iElem, elemIDs[iElem] + 1, elemCoefs[iElem]);
		}
		
		errCheck();
	}

	void SetRow(int iRow, int elemCount, double rowX, double rowB, vector<int> &elemIDs, vector<double> &elemCoefs) 
	{
		ENSURE(iRow >= 0);
		ENSURE(iRow < this->varCount);

		Q_SetLen(this->aMatPtr, iRow + 1, elemCount);
		V_SetCmp(this->xVecPtr, iRow + 1, rowX);
		V_SetCmp(this->bVecPtr, iRow + 1, rowB);
		for(int iElem = 0; iElem < elemCount; iElem++)
		{
			ENSURE(elemIDs[iElem] >= 0);
			ENSURE(elemIDs[iElem] < this->varCount);
			if(this->symmetric == true) 
			{
				ENSURE(elemIDs[iElem] >= iRow);
			}
			Q_SetEntry(this->aMatPtr, iRow + 1, iElem, elemIDs[iElem] + 1, elemCoefs[iElem]);
		}
		
		errCheck();
	}

	double GetSolution(int iRow)
	{
		double sol = V_GetCmp(this->xVecPtr, iRow + 1);
		errCheck();
		return sol;
	}

	SparseSolver(int varCount, bool symmetric);

	~SparseSolver();

	void DumpSystem();

	void Solve(double accuracy = 1e-5, int maxIter = 100);

	bool IsSymetric();
};

#endif
