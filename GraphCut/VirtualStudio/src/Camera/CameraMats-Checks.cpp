#include "CameraMats.h"

void CameraMats::CheckRotMat()
{
	ENSURE(this->cameraMatsLoaded == true);
	CTransform3x3 residual = this->rotMat.Inverse() + (this->rotMat.Transpose() * -1.0);
	for(int iRow = 0; iRow < 3; iRow++)
	for(int iCol = 0; iCol < 3; iCol++)
	{
		ENSURE(fabs(residual[iRow][iCol]) <= 0.0001);
	}
}
