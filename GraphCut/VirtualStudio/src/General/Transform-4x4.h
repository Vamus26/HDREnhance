#ifndef __TRANSFORM_4X4_H__
#define __TRANSFORM_4X4_H__

#include "Definitions.h"

class CVector4
{
public:
	CVector4(double a = 0.0, double b = 0.0, double c = 0.0, double d = 0.0);

	double &operator[](int i);              
	const double &operator[](int i) const;  
	void Dump() const;

private:
	double m_array[4];
};

inline double &CVector4::operator[](int i)
{
    return m_array[i];
}

inline const double &CVector4::operator[](int i) const
{
    return m_array[i];
}


class CTransform4x4
{
public:
    CTransform4x4();                          // default constructor (identity)	
    double* operator[](int i);                // access the elements
    const double* operator[](int i) const;    // access the elements
	CTransform4x4 operator*(const CTransform4x4& m) const;
	CVector4 operator*(const CVector4& v) const;
	bool operator==(const CTransform4x4& m) const;

	CTransform4x4 Inverse(void);              // matrix inverse
	static CTransform4x4 Identity();
	static CTransform4x4 Null();

	void Write(ofstream &outStream) const;
	void Read(ifstream &inStream);

	void Dump() const;
private:
    double m_array[4][4];                     // data array

	void exchangeRows(int row1, int row2);
	void scaleRow(int iRow, double scale);
	void addScaledRow(int iSrc, int iTarget, double scale);
};



inline double* CTransform4x4::operator[](int i)
{
    return m_array[i];
}

inline const double* CTransform4x4::operator[](int i) const
{
    return m_array[i];
}


#endif


