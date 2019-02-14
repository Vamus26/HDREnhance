#include "SparseSolver.h"


SparseSolver::SparseSolver(int varCount, bool symmetric)
{
	this->varCount  = varCount;
	this->symmetric = symmetric;

	Boolean symmetry = (this->symmetric == true) ? True : False;
	Q_Constr(&(this->aMat), "A", this->varCount, symmetry, Rowws, Normal, True);
	V_Constr(&(this->xVec), "x", this->varCount, Normal, True);
	V_Constr(&(this->bVec), "b", this->varCount, Normal, True);	

	this->aMatPtr = &(this->aMat);
	this->xVecPtr = &(this->xVec);
	this->bVecPtr = &(this->bVec);

	errCheck();
}

SparseSolver::~SparseSolver() 
{
	Q_Destr(this->aMatPtr);
	V_Destr(this->xVecPtr);
	V_Destr(this->bVecPtr);

	errCheck();
}

void SparseSolver::Solve(double accuracy, int maxIter) 
{
	printf("Solving System...\n");
	
	SetRTCAccuracy(accuracy); 
	CGIter(this->aMatPtr, 
		   this->xVecPtr, 
		   this->bVecPtr, 
		   maxIter, 
		   SSORPrecond, 
		   1.2);

	printf("\tDone - %lf.\n", GetLastAccuracy());

	errCheck();
}

void SparseSolver::DumpSystem()
{
	printf("System: %s\n", Q_GetName(aMatPtr));		
	for(int iRow = 1; iRow <= this->varCount; iRow++)
	{
		printf("[ ");
		for(int iCol = 1; iCol <= this->varCount; iCol++)
		{
			printf("%.02lf ", Q_GetEl(aMatPtr, iRow, iCol));
		}
		printf("][ %.02lf ] = [ %.02lf ]\n", 
			V_GetCmp(xVecPtr, iRow), 
			V_GetCmp(bVecPtr, iRow));
	}
	errCheck();
}

bool SparseSolver::IsSymetric()
{
	for(int iRow = 1; iRow < this->varCount; iRow++)
	{		
		for(int iCol = 1; iCol < this->varCount; iCol++)
		{
			double a  = Q_GetEl(aMatPtr,  iRow, iCol);
			double aa = Q_GetEl(aMatPtr,  iCol, iRow);
			errCheck();
			if(aa != a)
			{
				return false;
			}
		}			
	}
	return true;
}

