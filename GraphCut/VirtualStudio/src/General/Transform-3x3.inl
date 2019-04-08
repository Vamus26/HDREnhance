
#include "Transform-3x3.h"
#include <stdio.h>
#include "Debug.h"

#ifndef M_PI 
#define M_PI       3.14159265358979323846
#endif 

template <class T>
Vector3<T>::Vector3(T a, T b, T c)
{
	m_array[0] = a;
	m_array[1] = b;
	m_array[2] = c;
}


template <class T>
Vector3<T> Vector3<T>::operator/(T scalar) const
{
	Vector3<T> result;

	result[0] = this->m_array[0] / scalar;
	result[1] = this->m_array[1] / scalar;
	result[2] = this->m_array[2] / scalar;

	return result;
}

template <class T>
Vector3<T> Vector3<T>::operator*(T scalar) const
{
	Vector3<T> result;

	result[0] = this->m_array[0] * scalar;
	result[1] = this->m_array[1] * scalar;
	result[2] = this->m_array[2] * scalar;

	return result;
}

template <class T>
Vector3<T> Vector3<T>::cross(const Vector3<T> &v) const {
	// Cross product
	//a × b = [a2b3 - a3b2, a3b1 - a1b3, a1b2 - a2b1]. 

	Vector3<T> result;

	result[0] = this->m_array[1]*v[2] - this->m_array[2]*v[1];
	result[1] = this->m_array[2]*v[0] - this->m_array[0]*v[2];
	result[2] = this->m_array[0]*v[1] - this->m_array[1]*v[0];

	return result;
}

template <class T>
Vector3<T> Vector3<T>::Normalize() const
{
	// Normalize vector
	Vector3<T> result;
	T len = Length();
	for (int i=0; i<3; i++)
		result[i] = this->m_array[i] / len;

	return result;
}

template <class T>
Transform3x3<T> Vector3<T>::operator*(const Vector3<T> &v) const
{
	// Vector outer product
	Transform3x3<T> result;
	for (int i=0; i<3; i++)
	for (int j=0; j<3; j++)
	{
		result[i][j] = this->m_array[i] * v[j];
	}

	return result;
}

template <class T>
T Vector3<T>::dot(const Vector3<T> &v) const 
{
	// Dot product
	T result = 0;
	for (int i=0; i<3; i++)
		result += this->m_array[i]*v[i];

	return result;
}

template <class T>
Vector3<T> Vector3<T>::operator+(const Vector3<T> &v) const
{
	Vector3<T> returnVec(this->m_array[0] + v[0], 
				         this->m_array[1] + v[1], 
					     this->m_array[2] + v[2]);
	return returnVec;
}

template <class T>
Vector3<T> Vector3<T>::operator-(const Vector3<T> &v) const
{
	Vector3<T> returnVec(this->m_array[0] - v[0], 
				         this->m_array[1] - v[1], 
						 this->m_array[2] - v[2]);
	return returnVec;
}

template <class T>
T Vector3<T>::Length() const
{
	T length = 0;
	for (int i=0; i<3; i++)
		length += this->m_array[i] * this->m_array[i];

	length = sqrt(length);

	return length;
}

template <class T>
void Vector3<T>::Dump() const
{
	printf("[%lf %lf %lf]\n", (double) m_array[0], (double) m_array[1], (double) m_array[2]);
}

template <class T>
void Vector3<T>::DumpSci() const
{
	printf("[%e %e %e]\n", (double) m_array[0], (double) m_array[1], (double) m_array[2]);
}


template <class T>
void Vector3<T>::Write(ofstream &outStream) const
{
	outStream << "\n";
	outStream << this->m_array[0] << " " << this->m_array[1] << " " << this->m_array[2] << "\n";
	outStream << "\n";
}

template <class T>
void Vector3<T>::Read(ifstream &inStream)
{
	inStream >> this->m_array[0];
	inStream >> this->m_array[1];
	inStream >> this->m_array[2];
}

template <class T>
Transform3x3<T>::Transform3x3()
{
    // Default constructor (identity)
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
	{
		this->m_array[i][j] = (T) (i == j);
	}
}


template <class T>
Transform3x3<T> Transform3x3<T>::Identity()
{
	Transform3x3<T> identityMat;
    
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
	{
		identityMat[i][j] = (T) (i == j);
	}

	return identityMat;
}

template <class T>
Transform3x3<T> Transform3x3<T>::Null()
{
	Transform3x3<T> nullMat;
    
    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
	{
		nullMat[i][j] = (T) 0;
	}

	return nullMat;
}

template <class T>
Transform3x3<T> Transform3x3<T>::Translation(T tx, T ty)
{
    // Translation matrix
    Transform3x3<T> M = Transform3x3<T>::Identity();
    M[0][2] = tx;
    M[1][2] = ty;
    return M;
}

template <class T>
Transform3x3<T> Transform3x3<T>::Scale(T sx, T sy)
{
	Transform3x3<T> M = Transform3x3<T>::Identity();
    M[0][0] = sx;
    M[1][1] = sy;
    return M;
}

template <class T>
Transform3x3<T> Transform3x3<T>::Rotation(T degrees)
{
    // Rotation matrix
    Transform3x3<T> M;
    T iQuad1 = fmod(fmod(degrees / 90, 4) + 4 + 0.5, 4) - 0.5;
    T iQuad0 = iQuad1 - (int)(iQuad1 + 0.5);
    T rad    = iQuad0 * M_PI / 4.0;
    T c      = cos(rad);
    T s      = sin(rad);
    while (iQuad0 < iQuad1)
    {
        T t = c; c = -s; s = t;
        iQuad0 += 1;
    }
    M[0][0] = c, M[0][1] = -s;
    M[1][0] = s, M[1][1] = c;
    return M;
}

template <class T>
Transform3x3<T> Transform3x3<T>::Rotation_X(T rads)
{
    Transform3x3<T> M = Identity();

    M[1][1] =  cos(rads);
    M[1][2] =  sin(rads);
    M[2][1] = -sin(rads);
    M[2][2] =  cos(rads);

    return M;
}


template <class T>
Transform3x3<T> Transform3x3<T>::Rotation_Y(T rads)
{
    Transform3x3<T> M = Identity();

    M[0][0] =  cos(rads);
    M[0][2] = -sin(rads);
    M[2][0] =  sin(rads);
    M[2][2] =  cos(rads);

    return M;
}

template <class T>
Transform3x3<T> Transform3x3<T>::Rotation_Z(T rads)
{
    Transform3x3<T> M = Identity();

    M[0][0] =  cos(rads);
    M[0][1] =  sin(rads);
    M[1][0] = -sin(rads);
    M[1][1] =  cos(rads);

    return M;
}

template <class T>
Transform3x3<T> Transform3x3<T>::Transpose(void) const
{
	// Matrix transpose, just swap rows and columns

	Transform3x3<T> result;
	for (int i=0; i<3; i++)
	for (int j=0; j<3; j++)
	{
		result[i][j] = this->m_array[j][i];
	}

	return result;
}

template <class T>
void Transform3x3<T>::exchangeRows(int row1, int row2)
{
	CHECK_RANGE(row1, 0, 2);
	CHECK_RANGE(row2, 0, 2);

	for(int iCol = 0; iCol < 3; iCol++)
	{
		T temp                    = this->m_array[row1][iCol];
		this->m_array[row1][iCol] = this->m_array[row2][iCol];
		this->m_array[row2][iCol] = temp;
	}
}

template <class T>
void Transform3x3<T>::scaleRow(int iRow, T scale)
{
	CHECK_RANGE(iRow, 0, 2);

	for(int iCol = 0; iCol < 3; iCol++)
	{
		this->m_array[iRow][iCol] *= scale;
	}
}

template <class T>
void Transform3x3<T>::addScaledRow(int iSrc, int iTarget, T scale)
{
	CHECK_RANGE(iSrc,    0, 2);
	CHECK_RANGE(iTarget, 0, 2);

	for(int iCol = 0; iCol < 3; iCol++)
	{
		T rowVal = this->m_array[iSrc][iCol];
		this->m_array[iTarget][iCol] += rowVal * scale;
	}
}

template <class T>
Transform3x3<T> Transform3x3<T>::Inverse() const
{
	Transform3x3<T> mi = Transform3x3<T>::Identity();
	Transform3x3<T> m  = *this;

    for (int iCol = 0; iCol < 3; iCol++)
    {
		int iPivotRow = -1;
        T maxPivot = 0.0;
        for (int iRow = iCol; iRow < 3; iRow++)
        {
            // find maximum row element to pivot on
            T currPivot = fabs(m[iRow][iCol]);
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

        T pivot = m[iCol][iCol];
		ENSURE(fabs(pivot) == maxPivot);
        m.scaleRow(iCol, (1 / pivot));
        mi.scaleRow(iCol, (1 / pivot));

		for (int iRow = 0; iRow < 3; iRow++)
		{
			if (iRow != iCol)
            {
                T scale = -m[iRow][iCol];
                m.addScaledRow(iCol, iRow, scale);
                mi.addScaledRow(iCol, iRow, scale);
            }
		}
    }

    return mi;
}

template <class T>
Transform3x3<T> Transform3x3<T>::operator*(T d) {
	// Multiply matrix by scalar

	Transform3x3<T> result;
	for (int i=0; i<3; i++)
	for (int j=0; j<3; j++)
	{
		result[i][j] = this->m_array[i][j] * d;
	}

	return result;
}

template <class T>
Vector3<T> Transform3x3<T>::operator*(const Vector3<T> &v) const
{
	// Transform vector
	Vector3<T> result;
	for (int i=0; i < 3; i++)
	{
		T sum = 0;
		for (int k = 0; k < 3; k++)
			sum += this->m_array[i][k] * v[k];
		result[i] = sum;
	}
	return result;
}

template <class T>
Transform3x3<T> Transform3x3<T>::operator*(const Transform3x3<T> &M) const
{
    // Matrix multiply
    const int n = 3;
	Transform3x3<T> result;
    for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
    {
		T sum = 0;
		for (int k = 0; k < n; k++)
			sum += this->m_array[i][k] * M[k][j];
		result[i][j] = sum;
    }

    return result;
}

template <class T>
Transform3x3<T> Transform3x3<T>::operator+(const Transform3x3<T> &M) const
{
    // Matrix addition
    const int n = 3;
	Transform3x3<T> result;
    for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
    {
		result[i][j] = this->m_array[i][j] + M[i][j];
    }

    return result;
}

template <class T>
bool Transform3x3<T>::operator==(const Transform3x3<T> &M) const
{
    const int n = 3;
    for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
    {
		if(fabs(this->m_array[i][j] - M[i][j]) > DBL_EPSILON)
		{
			return false;
		}
    }

    return true;
}


template <class T>
void Transform3x3<T>::Dump() const
{
	printf(" %lf %lf %lf \n %lf %lf %lf \n %lf %lf %lf \n", 
			(double) m_array[0][0], (double) m_array[0][1], (double) m_array[0][2], 
			(double) m_array[1][0], (double) m_array[1][1], (double) m_array[1][2], 
			(double) m_array[2][0], (double) m_array[2][1], (double) m_array[2][2]);
}

template <class T>
void Transform3x3<T>::DumpSci() const
{
	printf(" %e %e %e \n %e %e %e \n %e %e %e \n", 
			(double) m_array[0][0], (double) m_array[0][1], (double) m_array[0][2], 
			(double) m_array[1][0], (double) m_array[1][1], (double) m_array[1][2], 
			(double) m_array[2][0], (double) m_array[2][1], (double) m_array[2][2]);
}


template <class T>
void Transform3x3<T>::Write(ofstream &outStream) const
{
	outStream << "\n";
	outStream << this->m_array[0][0] << " " << this->m_array[0][1] << " " << this->m_array[0][2] << "\n";
	outStream << this->m_array[1][0] << " " << this->m_array[1][1] << " " << this->m_array[1][2] << "\n";
	outStream << this->m_array[2][0] << " " << this->m_array[2][1] << " " << this->m_array[2][2] << "\n";
	outStream << "\n";
}

template <class T>
void Transform3x3<T>::Read(ifstream &inStream)
{
	inStream >> this->m_array[0][0];
	inStream >> this->m_array[0][1];
	inStream >> this->m_array[0][2];
	inStream >> this->m_array[1][0];
	inStream >> this->m_array[1][1];
	inStream >> this->m_array[1][2];
	inStream >> this->m_array[2][0];
	inStream >> this->m_array[2][1];
	inStream >> this->m_array[2][2];
}


