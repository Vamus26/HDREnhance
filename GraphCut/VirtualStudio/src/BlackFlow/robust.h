#ifndef __ROBUST_H__
#define __ROBUST_H__

#include <stdlib.h>
#include <stdio.h>
/*#include <io.h>*/
#pragma warning(disable : 4005)
#pragma warning(disable : 4029)
#pragma warning(disable : 4013)
#pragma warning(disable : 4267)
#pragma warning(disable : 4057)


# define        DIM             512
# define        DIMH            1024
# define        FILTER          100
# define        PI              3.141592654
# define        ROUND(A)        ((A)<0.0?-((int)(-(A)+0.5)):((int)((A)+0.5)))
# define        max(A,B)        ((A)<(B)?(B):(A))
# define        min(A,B)        ((A)<(B)?(A):(B))
# define        sign(A,B)        ((A)<0?-(B):(B))
# define        abs(A)        ((A)<0?-(A):(A))

#define ROOT2 1.4142136f
#define MAXLEN 256


#pragma warning(disable : 4131)
#pragma warning(disable : 4144)
#pragma warning(disable : 4210)
#pragma warning(disable : 4100)
#pragma warning(disable : 4244)
#pragma warning(disable : 4101)
#pragma warning(disable : 4305)
#pragma warning(disable : 4189)


#endif
