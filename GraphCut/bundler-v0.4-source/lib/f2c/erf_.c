#include "f2c.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef REAL
#define REAL double
#endif

#ifdef KR_headers
double erf2();
REAL erf_(x) real *x;
#else
extern double erf2(double);
REAL erf_(real *x)
#endif
{
return( erf2((double)*x) );
}
#ifdef __cplusplus
}
#endif
