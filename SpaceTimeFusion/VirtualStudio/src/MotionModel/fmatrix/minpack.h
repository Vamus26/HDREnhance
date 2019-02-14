/* minpack.h */
/* Prototypes for FORTRAN minpack routines */

#ifndef __minpack_h__
#define __minpack_h__

void LMDIF1(void *fcn, const int *m, const int *n, double *x, double *fvec,
	     double *tol, int *info, int *iwa, double *wa, const int *lwa);

void lmdif1_(void *fcn, const int *m, const int *n, double *x, double *fvec,
	     double *tol, int *info, int *iwa, double *wa, const int *lwa);

void LMDIF(void *fcn, int *m, int *n, double *x, double *fvec, double *ftol, 
	    double *xtol, double *gtol, int *maxfev, double *epsfcn, double *diag,
	    int *mode, double *factor, int *nprint, int *info, int *nfev, double *fjac,
	    int *ldfjac, int *ipvt, double *qtf, double *wa1, double *wa2, double *wa3,
	    double *wa4);

void lmdif_(void *fcn, int *m, int *n, double *x, double *fvec, double *ftol, 
	    double *xtol, double *gtol, int *maxfev, double *epsfcn, double *diag,
	    int *mode, double *factor, int *nprint, int *info, int *nfev, double *fjac,
	    int *ldfjac, int *ipvt, double *qtf, double *wa1, double *wa2, double *wa3,
	    double *wa4);


#ifdef _WIN32
#define lmdif1_ LMDIF1
#define lmdif_ LMDIF
#endif


#endif /* __minpack_h__ */
