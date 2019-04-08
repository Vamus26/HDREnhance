/* fmatrix_pro.h */
/* Routines for estimating the fundamental matrix of a pair of images */

#ifndef __fmatrix_pro_h__
#define __fmatrix_pro_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define isnan _isnan
#endif

#include "vector.h"

/* Compute the epipoles of an F-matrix */
void fmatrix_compute_epipoles(double *F, double *e1, double *e2);

/* Compute the distance from l to the epipolar line of r under F */
double fmatrix_compute_residual(double *F, v3_t r, v3_t l);

/* Use RANSAC to estimate an F-matrix */
void estimate_fmatrix_ransac_matches(int num_pts, v3_t *a_pts, v3_t *b_pts, 
				     int num_trials, double threshold, 
				     double *F);

/* Use linear least-squares to estimate the fundamantal matrix.  The
 * F-matrix is returned in Fout, and the two epipoles in e1 and e2 */

void estimate_fmatrix_linear(int num_pts, v3_t *r_pts, v3_t *l_pts, 
			                 double *Fout, double *e1, double *e2);

void estimate_fmatrix(int num_pts, v3_t *r_pts, v3_t *l_pts, double *Fout, int nonLinearRefine);


/* Estimate the essential matrix from an F-matrix, assuming 
 * same focal lengths */
void estimate_essential_same_focal_lengths(double *F, double *alpha, double *E);

/* Estimate the essential matrix from an F-matrix, assuming 
 * different focal lengths */
void estimate_essential_different_focal_lengths(double *F, 
						double *calib1, double *calib2,
						double *E);
    
/* Find the closest rank 2 matrix to the given 3x3 matrix */
void closest_rank2_matrix(double *Fin, double *Fout, double *U, double *VT);

/* Refine an F-matrix estimate using LM */
void refine_fmatrix_nonlinear_matches(int num_pts, v3_t *r_pts, v3_t *l_pts, 
				      double *F0, double *Fout);

/* Compute an F-matrix from two sets of camera parameters */
void fmatrix_from_parameters(double *i0, double *R0, double *t0, double *i1, double *R1, double *t1, double *F);

#ifdef __cplusplus
}
#endif

#endif /* __fmatrix_h__ */
