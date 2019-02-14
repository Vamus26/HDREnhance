#ifndef __TRANSFORM_2X2_H__
#define __TRANSFORM_2X2_H__

#include "Definitions.h"

template <class T>
class Vector2;


template <class T>
class Transform2x2
{
public:
    Transform2x2();
    T* operator[](int i);                // access the elements
    const T* operator[](int i) const;    // access the elements
	T Det() const;
    Transform2x2<T> Inverse(void) const; // matrix inverse
	static Transform2x2<T> Null(void); 
	Vector2<T> operator*(const Vector2<T> &v) const;	
	void Dump() const;
	void DumpSci() const;

private:
	T m_array[2][2];
};

template <class T>
inline T* Transform2x2<T>::operator[](int i)
{
    return this->m_array[i];
}

template <class T>
inline const T* Transform2x2<T>::operator[](int i) const
{
    return this->m_array[i];
}

template <class T>
class Vector2
{
public:

	Vector2(T a = 0, T b = 0);
	T &operator[](int i);              
	const T &operator[](int i) const;  

	T Dist(const Vector2<T> &v) const;

	Vector2<T> operator-(const Vector2<T> &v) const;
	Vector2<T> operator+(const Vector2<T> &v) const;
	Vector2<T> operator/(T scalar) const;
	Vector2<T> operator*(T scalar) const;	
	bool operator!=(const Vector2<T> &v) const;

	void Dump() const;
	void DumpSci() const;

	static Vector2<T> Null();

private:
	T m_array[2];
};

template <class T>
inline T &Vector2<T>::operator[](int i)
{
    return m_array[i];
}

template <class T>
inline const T &Vector2<T>::operator[](int i) const
{
    return m_array[i];
}

typedef Vector2<double> CVector2;


#include "Transform-2x2.inl"


#endif

