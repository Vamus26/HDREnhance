#include "Transform-2x2.h"

template <class T>
Vector2<T>::Vector2(T a, T b)
{
	this->m_array[0] = a;
	this->m_array[1] = b;
}


template <class T>
Vector2<T> Vector2<T>::Null()
{
	static Vector2<T> nullVec(0, 0);

	return nullVec;
}


template <class T>
T Vector2<T>::Dist(const Vector2<T> &v) const
{
	T diff;
	T dist;

	diff  = this->m_array[0] - v[0];
	dist  = diff * diff;
	diff  = this->m_array[1] - v[1];
	dist += diff * diff;
	dist = sqrt(dist);

	return dist;
}

template <class T>
Vector2<T> Vector2<T>::operator-(const Vector2<T> &v) const
{
	Vector2<T> result;

	result[0] = this->m_array[0] - v[0];
	result[1] = this->m_array[1] - v[1];

	return result;
}


template <class T>
Vector2<T> Vector2<T>::operator+(const Vector2<T> &v) const
{
	Vector2<T> result;

	result[0] = this->m_array[0] + v[0];
	result[1] = this->m_array[1] + v[1];

	return result;
}



template <class T>
Vector2<T> Vector2<T>::operator/(T scalar) const
{
	Vector2<T> result;

	result[0] = this->m_array[0] / scalar;
	result[1] = this->m_array[1] / scalar;

	return result;
}

template <class T>
Vector2<T> Vector2<T>::operator*(T scalar) const
{
	Vector2<T> result;

	result[0] = this->m_array[0] * scalar;
	result[1] = this->m_array[1] * scalar;

	return result;
}

template <class T>
bool Vector2<T>::operator!=(const Vector2<T> &v) const
{
	if((fabs(this->m_array[0] - v[0]) <= FLT_EPSILON) &&
	   (fabs(this->m_array[1] - v[1]) <= FLT_EPSILON))
	   return false;
	else
	   return true;
}

template <class T>
void Vector2<T>::Dump() const
{
	printf("[%lf %lf]\n", (double) m_array[0], (double) m_array[1]);
}

template <class T>
void Vector2<T>::DumpSci() const
{
	printf("[%e %e]\n", (double) m_array[0], (double) m_array[1]);
}


template <class T>
Transform2x2<T>::Transform2x2()
{
    // Default constructor (identity)
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            this->m_array[i][j] = (T) (i == j);
}

template <class T>
T Transform2x2<T>::Det() const
{
	return (this->m_array[0][0] * this->m_array[1][1]) - 
		   (this->m_array[0][1] * this->m_array[1][0]);
}

template <class T>
Transform2x2<T> Transform2x2<T>::Null(void)
{
	Transform2x2<T> nullMat;
	nullMat[0][0] = 0; 
	nullMat[0][1] = 0;
	nullMat[1][0] = 0;
	nullMat[1][1] = 0;
	return nullMat;
}

template <class T>
Transform2x2<T> Transform2x2<T>::Inverse(void) const
{
	T det = Det();

	if(det == 0.0)
	{
		return Transform2x2<T>::Null();
	}

	Transform2x2<T> result;
	result[0][0] =  this->m_array[1][1] / det;
	result[0][1] = -this->m_array[0][1] / det;
	result[1][0] = -this->m_array[1][0] / det;	
	result[1][1] =  this->m_array[0][0] / det;

	return result;
}

template <class T>
Vector2<T> Transform2x2<T>::operator*(const Vector2<T> &v) const
{
	Vector2<T> result;
	for (int i=0; i < 2; i++)
	{
		T sum = 0;
		for (int k = 0; k < 2; k++)
			sum += this->m_array[i][k] * v[k];
		result[i] = sum;
	}
	return result;
}

template <class T>
void Transform2x2<T>::Dump() const
{
	printf(" %lf %lf\n %lf %lf\n", 
			(double) this->m_array[0][0], (double) this->m_array[0][1], 
			(double) this->m_array[1][0], (double) this->m_array[1][1]);
}

template <class T>
void Transform2x2<T>::DumpSci() const
{
	printf(" %e %e\n %e %e\n", 
			(double) this->m_array[0][0], (double) this->m_array[0][1], 
			(double) this->m_array[1][0], (double) this->m_array[1][1]);
}


