/* file: mkl_vsl_functions.h */
/*
//                             INTEL CONFIDENTIAL
//  Copyright(C) 2006 Intel Corporation. All Rights Reserved.
//  The source code contained  or  described herein and all documents related to
//  the source code ("Material") are owned by Intel Corporation or its suppliers
//  or licensors.  Title to the  Material remains with  Intel Corporation or its
//  suppliers and licensors. The Material contains trade secrets and proprietary
//  and  confidential  information of  Intel or its suppliers and licensors. The
//  Material  is  protected  by  worldwide  copyright  and trade secret laws and
//  treaty  provisions. No part of the Material may be used, copied, reproduced,
//  modified, published, uploaded, posted, transmitted, distributed or disclosed
//  in any way without Intel's prior express written permission.
//  No license  under any  patent, copyright, trade secret or other intellectual
//  property right is granted to or conferred upon you by disclosure or delivery
//  of the Materials,  either expressly, by implication, inducement, estoppel or
//  otherwise.  Any  license  under  such  intellectual property  rights must be
//  express and approved by Intel in writing.
*/
/*
//++
//  User-level VSL function declarations
//--
*/

#ifndef __MKL_VSL_FUNCTIONS_H__
#define __MKL_VSL_FUNCTIONS_H__

#include "mkl_vsl_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
//++
//  EXTERNAL API MACROS.
//  Used to construct VSL function declaration. Change them if you are going to
//  provide different API for VSL functions.
//--
*/
#if defined(MKL_VSL_STDCALL)
  #define _Vsl_Api(rtype,name,arg)   extern rtype __stdcall name    arg;
  #define _vsl_api(rtype,name,arg)   extern rtype __stdcall name    arg;
  #define _VSL_API(rtype,name,arg)   extern rtype __stdcall name    arg;
#else
  #define _Vsl_Api(rtype,name,arg)   extern rtype __cdecl   name    arg;
  #define _vsl_api(rtype,name,arg)   extern rtype __cdecl   name    arg;
  #define _VSL_API(rtype,name,arg)   extern rtype __cdecl   name    arg;
#endif

/*
//++
//  VSL CONTINUOUS DISTRIBUTION GENERATOR FUNCTION DECLARATIONS.
//--
*/
/* Cauchy distribution */
_Vsl_Api(int,vdRngCauchy,(int  , VSLStreamStatePtr ,int  , double [],          \
 double  , double  ))
_VSL_API(int,VDRNGCAUCHY,(int *, VSLStreamStatePtr ,int *, double [],          \
 double *, double *))
_vsl_api(int,vdrngcauchy,(int *, VSLStreamStatePtr ,int *, double [],          \
 double *, double *))
_Vsl_Api(int,vsRngCauchy,(int  , VSLStreamStatePtr ,int  , float [],           \
 float  ,  float   ))
_VSL_API(int,VSRNGCAUCHY,(int *, VSLStreamStatePtr ,int *, float [],           \
 float *,  float * ))
_vsl_api(int,vsrngcauchy,(int *, VSLStreamStatePtr ,int *, float [],           \
 float *,  float * ))

/* Uniform distribution */
_Vsl_Api(int,vdRngUniform,(int  , VSLStreamStatePtr , int  , double [],        \
 double  , double  ))
_VSL_API(int,VDRNGUNIFORM,(int *, VSLStreamStatePtr , int *, double [],        \
 double *, double *))
_vsl_api(int,vdrnguniform,(int *, VSLStreamStatePtr , int *, double [],        \
 double *, double *))
_Vsl_Api(int,vsRngUniform,(int  , VSLStreamStatePtr , int  , float [],         \
 float  ,  float   ))
_VSL_API(int,VSRNGUNIFORM,(int *, VSLStreamStatePtr , int *, float [],         \
 float *,  float * ))
_vsl_api(int,vsrnguniform,(int *, VSLStreamStatePtr , int *, float [],         \
 float *,  float * ))

/* Gaussian distribution */
_Vsl_Api(int,vdRngGaussian,(int  , VSLStreamStatePtr , int  , double [],       \
 double  , double  ))
_VSL_API(int,VDRNGGAUSSIAN,(int *, VSLStreamStatePtr , int *, double [],       \
 double *, double *))
_vsl_api(int,vdrnggaussian,(int *, VSLStreamStatePtr , int *, double [],       \
 double *, double *))
_Vsl_Api(int,vsRngGaussian,(int  , VSLStreamStatePtr , int  , float [],        \
 float  ,  float   ))
_VSL_API(int,VSRNGGAUSSIAN,(int *, VSLStreamStatePtr , int *, float [],        \
 float *,  float * ))
_vsl_api(int,vsrnggaussian,(int *, VSLStreamStatePtr , int *, float [],        \
 float *,  float * ))

/* GaussianMV distribution */
_Vsl_Api(int,vdRngGaussianMV,(int  , void *, int  , double [], int  ,          \
 int  , double *, double *))
_VSL_API(int,VDRNGGAUSSIANMV,(int *, void *, int *, double [], int *,          \
 int *, double *, double *))
_vsl_api(int,vdrnggaussianmv,(int *, void *, int *, double [], int *,          \
 int *, double *, double *))
_Vsl_Api(int,vsRngGaussianMV,(int  , void *, int  , float [],  int  ,          \
 int  , float *,  float * ))
_VSL_API(int,VSRNGGAUSSIANMV,(int *, void *, int *, float [],  int *,          \
 int *, float *,  float * ))
_vsl_api(int,vsrnggaussianmv,(int *, void *, int *, float [],  int *,          \
 int *, float *,  float * ))

/* Exponential distribution */
_Vsl_Api(int,vdRngExponential,(int  , VSLStreamStatePtr , int  ,               \
 double [], double  , double  ))
_VSL_API(int,VDRNGEXPONENTIAL,(int *, VSLStreamStatePtr , int *,               \
 double [], double *, double *))
_vsl_api(int,vdrngexponential,(int *, VSLStreamStatePtr , int *,               \
 double [], double *, double *))
_Vsl_Api(int,vsRngExponential,(int  , VSLStreamStatePtr , int  ,               \
 float [],  float  ,  float   ))
_VSL_API(int,VSRNGEXPONENTIAL,(int *, VSLStreamStatePtr , int *,               \
 float [],  float *,  float * ))
_vsl_api(int,vsrngexponential,(int *, VSLStreamStatePtr , int *,               \
 float [],  float *,  float * ))

/* Laplace distribution */
_Vsl_Api(int,vdRngLaplace,(int  , VSLStreamStatePtr , int  , double [],        \
 double  , double  ))
_VSL_API(int,VDRNGLAPLACE,(int *, VSLStreamStatePtr , int *, double [],        \
 double *, double *))
_vsl_api(int,vdrnglaplace,(int *, VSLStreamStatePtr , int *, double [],        \
 double *, double *))
_Vsl_Api(int,vsRngLaplace,(int  , VSLStreamStatePtr , int  , float [],         \
 float  ,  float   ))
_VSL_API(int,VSRNGLAPLACE,(int *, VSLStreamStatePtr , int *, float [],         \
 float *,  float * ))
_vsl_api(int,vsrnglaplace,(int *, VSLStreamStatePtr , int *, float [],         \
 float *,  float * ))

/* Weibull distribution */
_Vsl_Api(int,vdRngWeibull,(int  , VSLStreamStatePtr , int  , double [],        \
 double  , double  , double  ))
_VSL_API(int,VDRNGWEIBULL,(int *, VSLStreamStatePtr , int *, double [],        \
 double *, double *, double *))
_vsl_api(int,vdrngweibull,(int *, VSLStreamStatePtr , int *, double [],        \
 double *, double *, double *))
_Vsl_Api(int,vsRngWeibull,(int  , VSLStreamStatePtr , int  , float [],         \
 float  ,  float  ,  float   ))
_VSL_API(int,VSRNGWEIBULL,(int *, VSLStreamStatePtr , int *, float [],         \
 float *,  float *,  float * ))
_vsl_api(int,vsrngweibull,(int *, VSLStreamStatePtr , int *, float [],         \
 float *,  float *,  float * ))

/* Rayleigh distribution */
_Vsl_Api(int,vdRngRayleigh,(int  , VSLStreamStatePtr , int  ,                  \
 double [], double  , double  ))
_VSL_API(int,VDRNGRAYLEIGH,(int *, VSLStreamStatePtr , int *,                  \
 double [], double *, double *))
_vsl_api(int,vdrngrayleigh,(int *, VSLStreamStatePtr , int *,                  \
 double [], double *, double *))
_Vsl_Api(int,vsRngRayleigh,(int  , VSLStreamStatePtr , int  ,                  \
 float [],  float  ,  float   ))
_VSL_API(int,VSRNGRAYLEIGH,(int *, VSLStreamStatePtr , int *,                  \
 float [],  float *,  float * ))
_vsl_api(int,vsrngrayleigh,(int *, VSLStreamStatePtr , int *,                  \
 float [],  float *,  float * ))

/* Lognormal distribution */
_Vsl_Api(int,vdRngLognormal,(int  , VSLStreamStatePtr , int  ,                 \
 double [], double  , double  , double  , double  ))
_VSL_API(int,VDRNGLOGNORMAL,(int *, VSLStreamStatePtr , int *,                 \
 double [], double *, double *, double *, double *))
_vsl_api(int,vdrnglognormal,(int *, VSLStreamStatePtr , int *,                 \
 double [], double *, double *, double *, double *))
_Vsl_Api(int,vsRngLognormal,(int  , VSLStreamStatePtr , int  ,                 \
 float [],  float  ,  float  ,  float  ,  float   ))
_VSL_API(int,VSRNGLOGNORMAL,(int *, VSLStreamStatePtr , int *,                 \
 float [],  float *,  float *,  float *,  float * ))
_vsl_api(int,vsrnglognormal,(int *, VSLStreamStatePtr , int *,                 \
 float [],  float *,  float *,  float *,  float * ))

/* Gumbel distribution */
_Vsl_Api(int,vdRngGumbel,(int  , VSLStreamStatePtr , int  , double [],         \
 double  , double  ))
_VSL_API(int,VDRNGGUMBEL,(int *, VSLStreamStatePtr , int *, double [],         \
 double *, double *))
_vsl_api(int,vdrnggumbel,(int *, VSLStreamStatePtr , int *, double [],         \
 double *, double *))
_Vsl_Api(int,vsRngGumbel,(int  , VSLStreamStatePtr , int  , float [],          \
 float  ,  float  ))
_VSL_API(int,VSRNGGUMBEL,(int *, VSLStreamStatePtr , int *, float [],          \
 float *,  float *))
_vsl_api(int,vsrnggumbel,(int *, VSLStreamStatePtr , int *, float [],          \
 float *,  float *))

/* Gamma distribution */
_Vsl_Api(int,vdRngGamma,(int  , VSLStreamStatePtr , int  , double [],          \
 double  , double  , double  ))
_VSL_API(int,VDRNGGAMMA,(int *, VSLStreamStatePtr , int *, double [],          \
 double *, double *, double *))
_vsl_api(int,vdrnggamma,(int *, VSLStreamStatePtr , int *, double [],          \
 double *, double *, double *))
_Vsl_Api(int,vsRngGamma,(int  , VSLStreamStatePtr , int  , float [],           \
 float  , float  , float  ))
_VSL_API(int,VSRNGGAMMA,(int *, VSLStreamStatePtr , int *, float [],           \
 float *, float *, float *))
_vsl_api(int,vsrnggamma,(int *, VSLStreamStatePtr , int *, float [],           \
 float *, float *, float *))

/* Beta distribution */
_Vsl_Api(int,vdRngBeta,(int  , VSLStreamStatePtr , int  , double [],           \
 double ,  double ,  double  ,  double  ))
_VSL_API(int,VDRNGBETA,(int *, VSLStreamStatePtr , int *, double [],           \
 double *, double *, double  *, double *))
_vsl_api(int,vdrngbeta,(int *, VSLStreamStatePtr , int *, double [],           \
 double *, double *, double  *, double *))
_Vsl_Api(int,vsRngBeta,(int  , VSLStreamStatePtr , int  , float [],            \
 float ,  float ,  float ,  float  ))
_VSL_API(int,VSRNGBETA,(int *, VSLStreamStatePtr , int *, float [],            \
 float *, float *, float *, float *))
_vsl_api(int,vsrngbeta,(int *, VSLStreamStatePtr , int *, float [],            \
 float *, float *, float *, float *))

/*
//++
//  VSL DISCRETE DISTRIBUTION GENERATOR FUNCTION DECLARATIONS.
//--
*/
/* Bernoulli distribution */
_Vsl_Api(int,viRngBernoulli,(int  ,VSLStreamStatePtr , int  , int [], double  ))
_VSL_API(int,VIRNGBERNOULLI,(int *,VSLStreamStatePtr , int *, int [], double *))
_vsl_api(int,virngbernoulli,(int *,VSLStreamStatePtr , int *, int [], double *))

/* Uniform distribution */
_Vsl_Api(int,viRngUniform,(int  , VSLStreamStatePtr , int  , int [],           \
int  , int  ))
_VSL_API(int,VIRNGUNIFORM,(int *, VSLStreamStatePtr , int *, int [],           \
 int *, int *))
_vsl_api(int,virnguniform,(int *, VSLStreamStatePtr , int *, int [],           \
 int *, int *))

/* UniformBits distribution */
_Vsl_Api(int,viRngUniformBits,(int  ,VSLStreamStatePtr , int  ,unsigned int []))
_VSL_API(int,VIRNGUNIFORMBITS,(int *,VSLStreamStatePtr , int *,unsigned int []))
_vsl_api(int,virnguniformbits,(int *,VSLStreamStatePtr , int *,unsigned int []))

/* Geometric distribution */
_Vsl_Api(int,viRngGeometric,(int  , VSLStreamStatePtr , int  , int [],double  ))
_VSL_API(int,VIRNGGEOMETRIC,(int *, VSLStreamStatePtr , int *, int [],double *))
_vsl_api(int,virnggeometric,(int *, VSLStreamStatePtr , int *, int [],double *))

/* Binomial distribution */
_Vsl_Api(int,viRngBinomial,(int  , VSLStreamStatePtr , int  , int [],          \
 int  , double  ))
_VSL_API(int,VIRNGBINOMIAL,(int *, VSLStreamStatePtr , int *, int [],          \
 int *, double *))
_vsl_api(int,virngbinomial,(int *, VSLStreamStatePtr , int *, int [],          \
 int *, double *))

/* Hypergeometric distribution */
_Vsl_Api(int,viRngHypergeometric,(int  , VSLStreamStatePtr , int  ,            \
 int [], int  , int  , int  ))
_VSL_API(int,VIRNGHYPERGEOMETRIC,(int *, VSLStreamStatePtr , int *,            \
 int [], int *, int *, int *))
_vsl_api(int,virnghypergeometric,(int *, VSLStreamStatePtr , int *,            \
 int [], int *, int *, int *))

/* Negbinomial distribution */
_Vsl_Api(int,viRngNegbinomial,(int  , VSLStreamStatePtr , int  ,               \
 int [], double, double ))
_VSL_API(int,VIRNGNEGBINOMIAL,(int *, VSLStreamStatePtr , int *,               \
 int [], double *, double *))
_vsl_api(int,virngnegbinomial,(int *, VSLStreamStatePtr , int *,               \
 int [], double *, double *))

/* Poisson distribution */
_Vsl_Api(int,viRngPoisson,(int  , VSLStreamStatePtr , int  , int [],           \
 double  ))
_VSL_API(int,VIRNGPOISSON,(int *, VSLStreamStatePtr , int *, int [],           \
 double *))
_vsl_api(int,virngpoisson,(int *, VSLStreamStatePtr , int *, int [],           \
 double *))

/* PoissonV distribution */
_Vsl_Api(int,viRngPoissonV,(int  , VSLStreamStatePtr , int  , int [],          \
 double []))
_VSL_API(int,VIRNGPOISSONV,(int *, VSLStreamStatePtr , int *, int [],          \
 double []))
_vsl_api(int,virngpoissonv,(int *, VSLStreamStatePtr , int *, int [],          \
 double []))


/*
//++
//  VSL SERVICE FUNCTION DECLARATIONS.
//--
*/
/* NewStream - stream creation/initialization */
_Vsl_Api(int,vslNewStream,(VSLStreamStatePtr* , int  , unsigned int  ))
_vsl_api(int,vslnewstream,(VSLStreamStatePtr* , int *, unsigned int *))
_VSL_API(int,VSLNEWSTREAM,(VSLStreamStatePtr* , int *, unsigned int *))

/* NewStreamEx - advanced stream creation/initialization */
_Vsl_Api(int,vslNewStreamEx, (VSLStreamStatePtr* , int  , int  ,               \
 const unsigned int[]))
_vsl_api(int,vslnewstreamex,(VSLStreamStatePtr* , int *, int *,                \
 const unsigned int[]))
_VSL_API(int,VSLNEWSTREAMEX,(VSLStreamStatePtr* , int *, int *,                \
 const unsigned int[]))

_Vsl_Api(int,vsliNewAbstractStream,(VSLStreamStatePtr* , int,                  \
 unsigned int[], iUpdateFuncPtr))
_vsl_api(int,vslinewabstractstream,(VSLStreamStatePtr* , int *,                \
 unsigned int[], iUpdateFuncPtr))
_VSL_API(int,VSLINEWABSTRACTSTREAM,(VSLStreamStatePtr* , int *,                \
 unsigned int[], iUpdateFuncPtr))

_Vsl_Api(int,vsldNewAbstractStream,(VSLStreamStatePtr* , int,                  \
double[], double , double, dUpdateFuncPtr ))
_vsl_api(int,vsldnewabstractstream,(VSLStreamStatePtr* , int *,                \
double[], double *, double *, dUpdateFuncPtr))
_VSL_API(int,VSLDNEWABSTRACTSTREAM,(VSLStreamStatePtr* , int *,                \
double[], double *, double *, dUpdateFuncPtr))

_Vsl_Api(int,vslsNewAbstractStream,(VSLStreamStatePtr* , int,                  \
float[], float , float, sUpdateFuncPtr))
_vsl_api(int,vslsnewabstractstream,(VSLStreamStatePtr* , int *,                \
float[], float *, float *, sUpdateFuncPtr))
_VSL_API(int,VSLSNEWABSTRACTSTREAM,(VSLStreamStatePtr* , int *,                \
float[], float *, float *, sUpdateFuncPtr))

/* DeleteStream - delete stream */
_Vsl_Api(int,vslDeleteStream,(VSLStreamStatePtr*))
_vsl_api(int,vsldeletestream,(VSLStreamStatePtr*))
_VSL_API(int,VSLDELETESTREAM,(VSLStreamStatePtr*))

/* CopyStream - copy all stream information */
_Vsl_Api(int,vslCopyStream,(VSLStreamStatePtr*, VSLStreamStatePtr))
_vsl_api(int,vslcopystream,(VSLStreamStatePtr*, VSLStreamStatePtr))
_VSL_API(int,VSLCOPYSTREAM,(VSLStreamStatePtr*, VSLStreamStatePtr))

/* CopyStreamState - copy stream state only */
_Vsl_Api(int,vslCopyStreamState,(VSLStreamStatePtr, VSLStreamStatePtr))
_vsl_api(int,vslcopystreamstate,(VSLStreamStatePtr, VSLStreamStatePtr))
_VSL_API(int,VSLCOPYSTREAMSTATE,(VSLStreamStatePtr, VSLStreamStatePtr))

/* LeapfrogStream - leapfrog method */
_Vsl_Api(int,vslLeapfrogStream,(VSLStreamStatePtr, int  , int  ))
_vsl_api(int,vslleapfrogstream,(VSLStreamStatePtr, int *, int *))
_VSL_API(int,VSLLEAPFROGSTREAM,(VSLStreamStatePtr, int *, int *))

/* SkipAheadStream - skip-ahead method */
_Vsl_Api(int,vslSkipAheadStream,(VSLStreamStatePtr, int  ))
_vsl_api(int,vslskipaheadstream,(VSLStreamStatePtr, int *))
_VSL_API(int,VSLSKIPAHEADSTREAM,(VSLStreamStatePtr, int *))

/* GetStreamStateBrng - get BRNG associated with given stream */
_Vsl_Api(int,vslGetStreamStateBrng,(VSLStreamStatePtr ))
_vsl_api(int,vslgetstreamstatebrng,(VSLStreamStatePtr *))
_VSL_API(int,VSLGETSTREAMSTATEBRNG,(VSLStreamStatePtr *))

/* GetNumRegBrngs - get number of registered BRNGs */
_Vsl_Api(int,vslGetNumRegBrngs,(void))
_vsl_api(int,vslgetnumregbrngs,(void))
_VSL_API(int,VSLGETNUMREGBRNGS,(void))

/* RegisterBrng - register new BRNG */
_Vsl_Api(int,vslRegisterBrng,(const VSLBRngProperties* ))
_vsl_api(int,vslregisterbrng,(const VSLBRngProperties* ))
_VSL_API(int,VSLREGISTERBRNG,(const VSLBRngProperties* ))

/* GetBrngProperties - get BRNG properties */
_vsl_api(int,vslGetBrngProperties,(int  , VSLBRngProperties* ))
_vsl_api(int,vslgetbrngproperties,(int *, VSLBRngProperties* ))
_VSL_API(int,VSLGETBRNGPROPERTIES,(int *, VSLBRngProperties* ))


_Vsl_Api(int,vslSaveStreamF,(VSLStreamStatePtr ,  char* ))
_vsl_api(int,vslsavestreamf,(VSLStreamStatePtr *, char* , int ))
_VSL_API(int,VSLSAVESTREAMF,(VSLStreamStatePtr *, char* , int ))

_Vsl_Api(int,vslLoadStreamF,(VSLStreamStatePtr *, char* ))
_vsl_api(int,vslloadstreamf,(VSLStreamStatePtr *, char* , int ))
_VSL_API(int,VSLLOADSTREAMF,(VSLStreamStatePtr *, char* , int ))


/*
//++
//  VSL CONVOLUTION AND CORRELATION FUNCTION DECLARATIONS.
//--
*/

_Vsl_Api(int,vsldConvNewTask,(VSLConvTaskPtr* , int ,  int ,  int [], int [], int []));
_vsl_api(int,vsldconvnewtask,(VSLConvTaskPtr* , int* , int* , int [], int [], int []));
_VSL_API(int,VSLDCONVNEWTASK,(VSLConvTaskPtr* , int* , int* , int [], int [], int []));

_Vsl_Api(int,vslsConvNewTask,(VSLConvTaskPtr* , int ,  int ,  int [], int [], int []));
_vsl_api(int,vslsconvnewtask,(VSLConvTaskPtr* , int* , int* , int [], int [], int []));
_VSL_API(int,VSLSCONVNEWTASK,(VSLConvTaskPtr* , int* , int* , int [], int [], int []));

_Vsl_Api(int,vsldCorrNewTask,(VSLCorrTaskPtr* , int ,  int ,  int [], int [], int []));
_vsl_api(int,vsldcorrnewtask,(VSLCorrTaskPtr* , int* , int* , int [], int [], int []));
_VSL_API(int,VSLDCORRNEWTASK,(VSLCorrTaskPtr* , int* , int* , int [], int [], int []));

_Vsl_Api(int,vslsCorrNewTask,(VSLCorrTaskPtr* , int ,  int ,  int [], int [], int []));
_vsl_api(int,vslscorrnewtask,(VSLCorrTaskPtr* , int* , int* , int [], int [], int []));
_VSL_API(int,VSLSCORRNEWTASK,(VSLCorrTaskPtr* , int* , int* , int [], int [], int []));


_Vsl_Api(int,vsldConvNewTask1D,(VSLConvTaskPtr* , int ,  int ,  int ,  int  ));
_vsl_api(int,vsldconvnewtask1d,(VSLConvTaskPtr* , int* , int* , int* , int* ));
_VSL_API(int,VSLDCONVNEWTASK1D,(VSLConvTaskPtr* , int* , int* , int* , int* ));

_Vsl_Api(int,vslsConvNewTask1D,(VSLConvTaskPtr* , int ,  int ,  int ,  int  ));
_vsl_api(int,vslsconvnewtask1d,(VSLConvTaskPtr* , int* , int* , int* , int* ));
_VSL_API(int,VSLSCONVNEWTASK1D,(VSLConvTaskPtr* , int* , int* , int* , int* ));

_Vsl_Api(int,vsldCorrNewTask1D,(VSLCorrTaskPtr* , int ,  int ,  int ,  int  ));
_vsl_api(int,vsldcorrnewtask1d,(VSLCorrTaskPtr* , int* , int* , int* , int* ));
_VSL_API(int,VSLDCORRNEWTASK1D,(VSLCorrTaskPtr* , int* , int* , int* , int* ));

_Vsl_Api(int,vslsCorrNewTask1D,(VSLCorrTaskPtr* , int ,  int ,  int ,  int  ));
_vsl_api(int,vslscorrnewtask1d,(VSLCorrTaskPtr* , int* , int* , int* , int* ));
_VSL_API(int,VSLSCORRNEWTASK1D,(VSLCorrTaskPtr* , int* , int* , int* , int* ));


_Vsl_Api(int,vsldConvNewTaskX,(VSLConvTaskPtr* , int ,  int ,  int [], int [], int [], double [], int []));
_vsl_api(int,vsldconvnewtaskx,(VSLConvTaskPtr* , int* , int* , int [], int [], int [], double [], int []));
_VSL_API(int,VSLDCONVNEWTASKX,(VSLConvTaskPtr* , int* , int* , int [], int [], int [], double [], int []));

_Vsl_Api(int,vslsConvNewTaskX,(VSLConvTaskPtr* , int ,  int ,  int [], int [], int [], float  [], int []));
_vsl_api(int,vslsconvnewtaskx,(VSLConvTaskPtr* , int* , int* , int [], int [], int [], float  [], int []));
_VSL_API(int,VSLSCONVNEWTASKX,(VSLConvTaskPtr* , int* , int* , int [], int [], int [], float  [], int []));

_Vsl_Api(int,vsldCorrNewTaskX,(VSLCorrTaskPtr* , int ,  int ,  int [], int [], int [], double [], int []));
_vsl_api(int,vsldcorrnewtaskx,(VSLCorrTaskPtr* , int* , int* , int [], int [], int [], double [], int []));
_VSL_API(int,VSLDCORRNEWTASKX,(VSLCorrTaskPtr* , int* , int* , int [], int [], int [], double [], int []));

_Vsl_Api(int,vslsCorrNewTaskX,(VSLCorrTaskPtr* , int ,  int ,  int [], int [], int [], float  [], int []));
_vsl_api(int,vslscorrnewtaskx,(VSLCorrTaskPtr* , int* , int* , int [], int [], int [], float  [], int []));
_VSL_API(int,VSLSCORRNEWTASKX,(VSLCorrTaskPtr* , int* , int* , int [], int [], int [], float  [], int []));


_Vsl_Api(int,vsldConvNewTaskX1D,(VSLConvTaskPtr* , int ,  int ,  int ,  int ,  double [], int  ));
_vsl_api(int,vsldconvnewtaskx1d,(VSLConvTaskPtr* , int* , int* , int* , int* , double [], int* ));
_VSL_API(int,VSLDCONVNEWTASKX1D,(VSLConvTaskPtr* , int* , int* , int* , int* , double [], int* ));

_Vsl_Api(int,vslsConvNewTaskX1D,(VSLConvTaskPtr* , int ,  int ,  int ,  int ,  float  [], int  ));
_vsl_api(int,vslsconvnewtaskx1d,(VSLConvTaskPtr* , int* , int* , int* , int* , float  [], int* ));
_VSL_API(int,VSLSCONVNEWTASKX1D,(VSLConvTaskPtr* , int* , int* , int* , int* , float  [], int* ));

_Vsl_Api(int,vsldCorrNewTaskX1D,(VSLCorrTaskPtr* , int ,  int ,  int ,  int ,  double [], int  ));
_vsl_api(int,vsldcorrnewtaskx1d,(VSLCorrTaskPtr* , int* , int* , int* , int* , double [], int* ));
_VSL_API(int,VSLDCORRNEWTASKX1D,(VSLCorrTaskPtr* , int* , int* , int* , int* , double [], int* ));

_Vsl_Api(int,vslsCorrNewTaskX1D,(VSLCorrTaskPtr* , int ,  int ,  int ,  int ,  float  [], int  ));
_vsl_api(int,vslscorrnewtaskx1d,(VSLCorrTaskPtr* , int* , int* , int* , int* , float  [], int* ));
_VSL_API(int,VSLSCORRNEWTASKX1D,(VSLCorrTaskPtr* , int* , int* , int* , int* , float  [], int* ));


_Vsl_Api(int,vslConvDeleteTask,(VSLConvTaskPtr* ));
_vsl_api(int,vslconvdeletetask,(VSLConvTaskPtr* ));
_VSL_API(int,VSLCONVDeleteTask,(VSLConvTaskPtr* ));

_Vsl_Api(int,vslCorrDeleteTask,(VSLCorrTaskPtr* ));
_vsl_api(int,vslcorrdeletetask,(VSLCorrTaskPtr* ));
_VSL_API(int,VSLCORRDeleteTask,(VSLCorrTaskPtr* ));


_Vsl_Api(int,vslConvCopyTask,(VSLConvTaskPtr* , VSLConvTaskPtr ));
_vsl_api(int,vslconvcopytask,(VSLConvTaskPtr* , VSLConvTaskPtr ));
_VSL_API(int,VSLCONVCopyTask,(VSLConvTaskPtr* , VSLConvTaskPtr ));

_Vsl_Api(int,vslCorrCopyTask,(VSLCorrTaskPtr* , VSLCorrTaskPtr ));
_vsl_api(int,vslcorrcopytask,(VSLCorrTaskPtr* , VSLCorrTaskPtr ));
_VSL_API(int,VSLCORRCopyTask,(VSLCorrTaskPtr* , VSLCorrTaskPtr ));


_Vsl_Api(int,vslConvSetMode,(VSLConvTaskPtr , int  ));
_vsl_api(int,vslconvsetmode,(VSLConvTaskPtr , int* ));
_VSL_API(int,VSLCONVSETMOME,(VSLConvTaskPtr , int* ));

_Vsl_Api(int,vslCorrSetMode,(VSLCorrTaskPtr , int  ));
_vsl_api(int,vslcorrsetmode,(VSLCorrTaskPtr , int* ));
_VSL_API(int,VSLCORRSETMODE,(VSLCorrTaskPtr , int* ));


_Vsl_Api(int,vslConvSetInternalPrecision,(VSLConvTaskPtr , int  ));
_vsl_api(int,vslconvsetinternalprecision,(VSLConvTaskPtr , int* ));
_VSL_API(int,VSLCONVSETINTERNALPRECISION,(VSLConvTaskPtr , int* ));

_Vsl_Api(int,vslCorrSetInternalPrecision,(VSLCorrTaskPtr , int  ));
_vsl_api(int,vslcorrsetinternalprecision,(VSLCorrTaskPtr , int* ));
_VSL_API(int,VSLCORRSETINTERNALPRECISION,(VSLCorrTaskPtr , int* ));


_Vsl_Api(int,vslConvSetStart,(VSLConvTaskPtr , int []));
_vsl_api(int,vslconvsetstart,(VSLConvTaskPtr , int []));
_VSL_API(int,VSLCONVSETSTART,(VSLConvTaskPtr , int []));

_Vsl_Api(int,vslCorrSetStart,(VSLCorrTaskPtr , int []));
_vsl_api(int,vslcorrsetstart,(VSLCorrTaskPtr , int []));
_VSL_API(int,VSLCORRSETSTART,(VSLCorrTaskPtr , int []));


_Vsl_Api(int,vslConvSetDecimation,(VSLConvTaskPtr , int []));
_vsl_api(int,vslconvsetdecimation,(VSLConvTaskPtr , int []));
_VSL_API(int,VSLCONVSETDECIMATION,(VSLConvTaskPtr , int []));

_Vsl_Api(int,vslCorrSetDecimation,(VSLCorrTaskPtr , int []));
_vsl_api(int,vslcorrsetdecimation,(VSLCorrTaskPtr , int []));
_VSL_API(int,VSLCORRSETDECIMATION,(VSLCorrTaskPtr , int []));


_Vsl_Api(int,vsldConvExec,(VSLConvTaskPtr , double [], int [], double [], int [], double [], int []));
_vsl_api(int,vsldconvexec,(VSLConvTaskPtr , double [], int [], double [], int [], double [], int []));
_VSL_API(int,VSLDCONVEXEC,(VSLConvTaskPtr , double [], int [], double [], int [], double [], int []));

_Vsl_Api(int,vslsConvExec,(VSLConvTaskPtr , float [],  int [], float [],  int [], float [],  int []));
_vsl_api(int,vslsconvexec,(VSLConvTaskPtr , float [],  int [], float [],  int [], float [],  int []));
_VSL_API(int,VSLSCONVEXEC,(VSLConvTaskPtr , float [],  int [], float [],  int [], float [],  int []));

_Vsl_Api(int,vsldCorrExec,(VSLCorrTaskPtr , double [], int [], double [], int [], double [], int []));
_vsl_api(int,vsldcorrexec,(VSLCorrTaskPtr , double [], int [], double [], int [], double [], int []));
_VSL_API(int,VSLDCORREXEC,(VSLCorrTaskPtr , double [], int [], double [], int [], double [], int []));

_Vsl_Api(int,vslsCorrExec,(VSLCorrTaskPtr , float [],  int [], float [],  int [], float [],  int []));
_vsl_api(int,vslscorrexec,(VSLCorrTaskPtr , float [],  int [], float [],  int [], float [],  int []));
_VSL_API(int,VSLSCORREXEC,(VSLCorrTaskPtr , float [],  int [], float [],  int [], float [],  int []));


_Vsl_Api(int,vsldConvExec1D,(VSLConvTaskPtr , double [], int ,  double [], int ,  double [], int  ));
_vsl_api(int,vsldconvexec1d,(VSLConvTaskPtr , double [], int* , double [], int* , double [], int* ));
_VSL_API(int,VSLDCONVEXEC1D,(VSLConvTaskPtr , double [], int* , double [], int* , double [], int* ));

_Vsl_Api(int,vslsConvExec1D,(VSLConvTaskPtr , float [],  int ,  float [],  int ,  float [],  int  ));
_vsl_api(int,vslsconvexec1d,(VSLConvTaskPtr , float [],  int* , float [],  int* , float [],  int* ));
_VSL_API(int,VSLSCONVEXEC1D,(VSLConvTaskPtr , float [],  int* , float [],  int* , float [],  int* ));

_Vsl_Api(int,vsldCorrExec1D,(VSLCorrTaskPtr , double [], int ,  double [], int ,  double [], int  ));
_vsl_api(int,vsldcorrexec1d,(VSLCorrTaskPtr , double [], int* , double [], int* , double [], int* ));
_VSL_API(int,VSLDCORREXEC1D,(VSLCorrTaskPtr , double [], int* , double [], int* , double [], int* ));

_Vsl_Api(int,vslsCorrExec1D,(VSLCorrTaskPtr , float [],  int ,  float [],  int ,  float [],  int  ));
_vsl_api(int,vslscorrexec1d,(VSLCorrTaskPtr , float [],  int* , float [],  int* , float [],  int* ));
_VSL_API(int,VSLSCORREXEC1D,(VSLCorrTaskPtr , float [],  int* , float [],  int* , float [],  int* ));


_Vsl_Api(int,vsldConvExecX,(VSLConvTaskPtr , double [], int [], double [], int []));
_vsl_api(int,vsldconvexecx,(VSLConvTaskPtr , double [], int [], double [], int []));
_VSL_API(int,VSLDCONVEXECX,(VSLConvTaskPtr , double [], int [], double [], int []));

_Vsl_Api(int,vslsConvExecX,(VSLConvTaskPtr , float [],  int [], float [],  int []));
_vsl_api(int,vslsconvexecx,(VSLConvTaskPtr , float [],  int [], float [],  int []));
_VSL_API(int,VSLSCONVEXECX,(VSLConvTaskPtr , float [],  int [], float [],  int []));

_Vsl_Api(int,vsldCorrExecX,(VSLCorrTaskPtr , double [], int [], double [], int []));
_vsl_api(int,vsldcorrexecx,(VSLCorrTaskPtr , double [], int [], double [], int []));
_VSL_API(int,VSLDCORREXECX,(VSLCorrTaskPtr , double [], int [], double [], int []));

_Vsl_Api(int,vslsCorrExecX,(VSLCorrTaskPtr , float [],  int [], float [],  int []));
_vsl_api(int,vslscorrexecx,(VSLCorrTaskPtr , float [],  int [], float [],  int []));
_VSL_API(int,VSLSCORREXECX,(VSLCorrTaskPtr , float [],  int [], float [],  int []));


_Vsl_Api(int,vsldConvExecX1D,(VSLConvTaskPtr , double [], int ,  double [], int  ));
_vsl_api(int,vsldconvexecx1d,(VSLConvTaskPtr , double [], int* , double [], int* ));
_VSL_API(int,VSLDCONVEXECX1D,(VSLConvTaskPtr , double [], int* , double [], int* ));

_Vsl_Api(int,vslsConvExecX1D,(VSLConvTaskPtr , float [],  int ,  float [],  int  ));
_vsl_api(int,vslsconvexecx1d,(VSLConvTaskPtr , float [],  int* , float [],  int* ));
_VSL_API(int,VSLSCONVEXECX1D,(VSLConvTaskPtr , float [],  int* , float [],  int* ));

_Vsl_Api(int,vsldCorrExecX1D,(VSLCorrTaskPtr , double [], int ,  double [], int  ));
_vsl_api(int,vsldcorrexecx1d,(VSLCorrTaskPtr , double [], int* , double [], int* ));
_VSL_API(int,VSLDCORREXECX1D,(VSLCorrTaskPtr , double [], int* , double [], int* ));

_Vsl_Api(int,vslsCorrExecX1D,(VSLCorrTaskPtr , float [],  int ,  float [],  int ));
_vsl_api(int,vslscorrexecx1d,(VSLCorrTaskPtr , float [],  int* , float [],  int* ));
_VSL_API(int,VSLSCORREXECX1D,(VSLCorrTaskPtr , float [],  int* , float [],  int* ));


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MKL_VSL_FUNCTIONS_H__ */
