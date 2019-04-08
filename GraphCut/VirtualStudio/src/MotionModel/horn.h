/* horn.h */
/* Compute the closed-form least-squares solution to a rigid body alignment */

#ifndef __horn_h__
#define __horn_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "vector.h"

/* Computes the closed-form least-squares solution to a rigid
 * body alignment.
 *
 * n: the number of points
 * right_pts: One set of n points (the set of matches)
 * left_pts:  The other set of n points (the set of input points)
 * R: on return, contains the optimal (2x2) rotation 
 * T: on return, contains the optimal (3x3) translation
 * weight: array of weights assigned to each pair of points.  Leave
 *         parameter NULL to weight all points equally */
double align_horn(int n, v3_t *right_pts, v3_t *left_pts, 
		  double *R, double *T, double *Tout, 
		  double *scale, double *weight);

/* Align two sets of points with a 3D rotation */
double align_3D_rotation(int n, v3_t *r_pts, v3_t *l_pts, double *R);

/* Align two sets of points with a 3D rotation */
int align_3D_rotation_ransac(int n, v3_t *r_pts, v3_t *l_pts, 
			     int num_ransac_rounds, double ransac_thresh,
			     double *R);
    
#ifdef __cplusplus
}
#endif

#endif /* __horn_h__ */
