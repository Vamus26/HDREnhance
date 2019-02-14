#include "Transform-4x4.h"
#include <fstream>

CTransform4x4::CTransform4x4()
{
    // Default constructor (identity)
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m_array[i][j] = (i == j);
}


CTransform4x4 CTransform4x4::operator*(const CTransform4x4& m) const
{
	const int n = 4;
	CTransform4x4 result;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            double sum = 0.0;
            for (int k = 0; k < n; k++)
			{
                sum += this->m_array[i][k] * m[k][j];
			}
            result[i][j] = sum;
        }
    }
    return result;
}

CVector4 CTransform4x4::operator*(const CVector4& v) const
{
	CVector4 result;
	for (int i=0; i < 4; i++)
	{
		double sum = 0.0;
		for (int k = 0; k < 4; k++)
		{
			sum += this->m_array[i][k] * v[k];
		}
		result[i] = sum;
	}
	return result;
}

bool CTransform4x4::operator==(const CTransform4x4& M1) const
{
    const int n = 4;
    const CTransform4x4& M0 = *this;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
			if(fabs(M0[i][j] - M1[i][j]) > DBL_EPSILON)
			{
				return false;
			}
        }
    }

    return true;
}


CTransform4x4 CTransform4x4::Identity()
{
	CTransform4x4 identityMat;
    
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            identityMat[i][j] = (i == j);

	return identityMat;
}

CTransform4x4 CTransform4x4::Null()
{
	CTransform4x4 nullMat;
    
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            nullMat[i][j] = 0.0;

	return nullMat;
}

void CTransform4x4::exchangeRows(int row1, int row2)
{
	CHECK_RANGE(row1, 0, 3);
	CHECK_RANGE(row2, 0, 3);

	for(int iCol = 0; iCol < 4; iCol++)
	{
		double temp               = this->m_array[row1][iCol];
		this->m_array[row1][iCol] = this->m_array[row2][iCol];
		this->m_array[row2][iCol] = temp;
	}
}

void CTransform4x4::scaleRow(int iRow, double scale)
{
	CHECK_RANGE(iRow, 0, 3);

	for(int iCol = 0; iCol < 4; iCol++)
	{
		this->m_array[iRow][iCol] *= scale;
	}
}

void CTransform4x4::addScaledRow(int iSrc, int iTarget, double scale)
{
	CHECK_RANGE(iSrc,    0, 3);
	CHECK_RANGE(iTarget, 0, 3);

	for(int iCol = 0; iCol < 4; iCol++)
	{
		double rowVal                 = this->m_array[iSrc][iCol];
		this->m_array[iTarget][iCol] += rowVal * scale;
	}
}

CTransform4x4 CTransform4x4::Inverse()
{
	CTransform4x4 mi = CTransform4x4::Identity();
	CTransform4x4 m  = *this;

	

    for (int iCol = 0; iCol < 4; iCol++)
    {
		int iPivotRow = -1;
        double maxPivot = 0.0;
        for (int iRow = iCol; iRow < 4; iRow++)
        {
            // find maximum row element to pivot on
            double currPivot = fabs(m[iRow][iCol]);
            if (currPivot > maxPivot)
            {
                iPivotRow = iRow;
                maxPivot = currPivot;
            }
        }

		if (maxPivot == 0.0)
        {
			REPORT_FAILURE("Matrix cannot be inverted");         
        }

        if (iPivotRow != iCol)
        {
            m.exchangeRows(iCol, iPivotRow);
            mi.exchangeRows(iCol, iPivotRow);
        }

        double pivot = m[iCol][iCol];
		ENSURE(fabs(pivot) == maxPivot);
        m.scaleRow(iCol, (1.0 / pivot));
        mi.scaleRow(iCol, (1.0 / pivot));

        for (int iRow = 0; iRow < 4; iRow++)
		{
			if (iRow != iCol)
            {
                double scale = -m[iRow][iCol];
                m.addScaledRow(iCol, iRow, scale);
                mi.addScaledRow(iCol, iRow, scale);
            }
		}
    }

    return mi;
}


void CTransform4x4::Write(ofstream &outStream) const
{
	outStream << "\n";
	outStream << this->m_array[0][0] << " " << this->m_array[0][1] << " " << this->m_array[0][2] << " " << this->m_array[0][3] << "\n";
	outStream << this->m_array[1][0] << " " << this->m_array[1][1] << " " << this->m_array[1][2] << " " << this->m_array[1][3] << "\n";
	outStream << this->m_array[2][0] << " " << this->m_array[2][1] << " " << this->m_array[2][2] << " " << this->m_array[2][3] << "\n";
	outStream << this->m_array[3][0] << " " << this->m_array[3][1] << " " << this->m_array[3][2] << " " << this->m_array[3][3] << "\n";
	outStream << "\n";
}

void CTransform4x4::Read(ifstream &inStream)
{
	inStream >> this->m_array[0][0];
	inStream >> this->m_array[0][1];
	inStream >> this->m_array[0][2];
	inStream >> this->m_array[0][3];
	inStream >> this->m_array[1][0];
	inStream >> this->m_array[1][1];
	inStream >> this->m_array[1][2];
	inStream >> this->m_array[1][3];
	inStream >> this->m_array[2][0];
	inStream >> this->m_array[2][1];
	inStream >> this->m_array[2][2];
	inStream >> this->m_array[2][3];
	inStream >> this->m_array[3][0];
	inStream >> this->m_array[3][1];
	inStream >> this->m_array[3][2];
	inStream >> this->m_array[3][3];
}


void CTransform4x4::Dump() const
{
	printf("%lf %lf %lf %lf \n%lf %lf %lf %lf \n%lf %lf %lf %lf \n%lf %lf %lf %lf \n", 
			m_array[0][0], m_array[0][1], m_array[0][2], m_array[0][3],
			m_array[1][0], m_array[1][1], m_array[1][2], m_array[1][3],
			m_array[2][0], m_array[2][1], m_array[2][2], m_array[2][3],
			m_array[3][0], m_array[3][1], m_array[3][2], m_array[3][3]);
}


CVector4::CVector4(double a, double b, double c, double d)
{
	this->m_array[0] = a;
	this->m_array[1] = b;
	this->m_array[2] = c;
	this->m_array[3] = d;
}

void CVector4::Dump() const
{
	printf("[%lf %lf %lf %lf]\n", m_array[0], m_array[1], m_array[2], m_array[3]);
}


