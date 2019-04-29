#include "f2c.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef KR_headers
double erf2();
double derf_(x) doublereal *x;
#else
extern double erf2(double);
double derf_(doublereal *x)
#endif
{
return( erf2(*x) );
}
#ifdef __cplusplus
}
#endif
