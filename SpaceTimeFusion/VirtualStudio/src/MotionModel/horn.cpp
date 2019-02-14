/* horn.c */
/* Compute the closed-form least-squares solution to a rigid body alignment */

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "horn.h"
#include "matrix.h"
#include "vector.h"

/* Computes the closed-form least-squares solution to a rigid
 * body alignment.
 *
 * n: the number of points
 * right_pts: Target set of n points 
 * left_pts:  Source set of n points */
double align_horn(int n, v3_t *right_pts, v3_t *left_pts, 
		  double *R, double *T, 
		  double *Tout, double *scale, double *weight) {
    int i;
    v3_t right_centroid = v3_new(0.0, 0.0, 0.0);
    v3_t left_centroid = v3_new(0.0, 0.0, 0.0);
    double M[2][2] = { { 0.0, 0.0 }, 
                       { 0.0, 0.0 } };
    double MT[2][2];
    double MTM[2][2];
    double eval[2], sqrteval[2];
    double evec[2][2];
    double S[2][2], Sinv[2][2], U[2][2];
    double Tcenter[3][3] = { { 1.0, 0.0, 0.0 },
			     { 0.0, 1.0, 0.0 },
			     { 0.0, 0.0, 1.0 } };

    double Ttmp[3][3];

    double sum_num, sum_den, RMS_sum;

	if(weight)
	{
		double weight_sum = 0.0;

		/* Compute the weighted centroid of both point sets */
		for (i = 0; i < n; i++) {
			right_centroid = v3_add(right_centroid, v3_scale(weight[i], right_pts[i]));
			left_centroid = v3_add(left_centroid, v3_scale(weight[i], left_pts[i]));

			weight_sum += weight[i];
		}

		right_centroid = v3_scale(1.0 / weight_sum, right_centroid);
		left_centroid = v3_scale(1.0 / weight_sum, left_centroid);
	}
	else
	{
		/* Calculate the centroid of both sets of points */
		for (i = 0; i < n; i++) {
			right_centroid = v3_add(right_centroid, right_pts[i]);
			left_centroid = v3_add(left_centroid, left_pts[i]);
		}

		right_centroid = v3_scale(1.0 / n, right_centroid);
		left_centroid = v3_scale(1.0 / n, left_centroid);
	}

    /* Compute the scale */
    sum_num = sum_den = 0.0;

    for (i = 0; i < n; i++) {
        v3_t r = v3_sub(right_centroid, right_pts[i]);
        v3_t l = v3_sub(left_centroid, left_pts[i]);
	
	sum_num = v3_magsq(r);
	sum_den = v3_magsq(l);
    }

    *scale = sqrt(sum_num / sum_den);

	/* Fill in the matrix M */
	for (i = 0; i < n; i++) {
		v3_t r = v3_sub(right_centroid, right_pts[i]);
		v3_t l = v3_sub(left_centroid, left_pts[i]);

		if (weight) {
			M[0][0] += Vx(r) * Vx(l);
			M[0][1] += Vx(r) * Vy(l);
			M[1][0] += Vy(r) * Vx(l);
			M[1][1] += Vy(r) * Vy(l);
		} else {
			M[0][0] += Vx(r) * Vx(l);
			M[0][1] += Vx(r) * Vy(l);
			M[1][0] += Vy(r) * Vx(l);
			M[1][1] += Vy(r) * Vy(l);
		}
	}

    /* Compute MTM */
    matrix_transpose(2, 2, (double *)M, (double *)MT);
    matrix_product(2, 2, 2, 2, (double *)MT, (double *)M, (double *)MTM);

    /* Calculate Sinv, the inverse of the square root of MTM */
    dgeev_driver(2, (double *)MTM, (double *)evec, eval);
    
    /* MTM = eval[0] * evec[0]T * evec[0] + eval[1] * evec[1]T * evec[1] */
    /* S = sqrt(eval[0]) * evec[0]T * evec[0] + sqrt(eval[1]) * evec[1]T * evec[1] */
    sqrteval[0] = sqrt(eval[0]);
    sqrteval[1] = sqrt(eval[1]);

    S[0][0] = 
        (sqrteval[0]) * evec[0][0] * evec[0][0] +
        (sqrteval[1]) * evec[1][0] * evec[1][0];
    S[0][1] = 
        (sqrteval[0]) * evec[0][0] * evec[0][1] +
        (sqrteval[1]) * evec[1][0] * evec[1][1];
    S[1][0] = 
        (sqrteval[0]) * evec[0][1] * evec[0][0] +
        (sqrteval[1]) * evec[1][1] * evec[1][0];
    S[1][1] = 
        (sqrteval[0]) * evec[0][1] * evec[0][1] +
        (sqrteval[1]) * evec[1][1] * evec[1][1];
    
    Sinv[0][0] = 
        (1.0 / sqrteval[0]) * evec[0][0] * evec[0][0] +
        (1.0 / sqrteval[1]) * evec[1][0] * evec[1][0];
    Sinv[0][1] = 
        (1.0 / sqrteval[0]) * evec[0][0] * evec[0][1] +
        (1.0 / sqrteval[1]) * evec[1][0] * evec[1][1];
    Sinv[1][0] = 
        (1.0 / sqrteval[0]) * evec[0][1] * evec[0][0] +
        (1.0 / sqrteval[1]) * evec[1][1] * evec[1][0];
    Sinv[1][1] = 
        (1.0 / sqrteval[0]) * evec[0][1] * evec[0][1] +
        (1.0 / sqrteval[1]) * evec[1][1] * evec[1][1];
    
    // matrix_product(2, 2, 2, 2, (double *)S, (double *)Sinv, (double *)U);

    /* U = M * Sinv */
    matrix_product(2, 2, 2, 2, (double *)M, (double *)Sinv, (double *)U);

    /* Fill in the rotation matrix */
    R[0] = U[0][0]; R[1] = U[0][1]; R[2] = 0.0;
    R[3] = U[1][0], R[4] = U[1][1]; R[5] = 0.0;
    R[6] = 0.0;     R[7] = 0.0;     R[8] = 1.0;

    // memcpy(R, U, sizeof(double) * 4);

    /* Fill in the translation matrix */
    T[0] = T[4] = T[8] = 1.0;
    T[1] = T[3] = T[6] = T[7] = 0.0;
    T[2] = Vx(right_centroid);
    T[5] = Vy(right_centroid);

    Tcenter[0][2] = -Vx(left_centroid);
    Tcenter[1][2] = -Vy(left_centroid);

    matrix_product(3, 3, 3, 3, T, R, (double *)Ttmp);

#if 0
#if 0
    /* Do the scaling */
    Ttmp[0][0] *= *scale;
    Ttmp[0][1] *= *scale;
    Ttmp[0][2] *= *scale;
    Ttmp[1][0] *= *scale;
    Ttmp[1][1] *= *scale;
    Ttmp[1][2] *= *scale;
#else
    Tcenter[0][0] *= *scale;
    Tcenter[0][2] *= *scale;
    Tcenter[1][1] *= *scale;
    Tcenter[1][2] *= *scale;
#endif
#endif

    matrix_product(3, 3, 3, 3, (double *)Ttmp, (double *)Tcenter, Tout);

    T[2] = Vx(v3_sub(right_centroid, left_centroid));
    T[5] = Vy(v3_sub(right_centroid, left_centroid));


    /* Now compute the RMS error between the points */
    RMS_sum = 0.0;

    for (i = 0; i < n; i++) {
        v3_t r = v3_sub(right_centroid, right_pts[i]);
        v3_t l = v3_sub(left_centroid, left_pts[i]);
	v3_t resid;

	/* Rotate, scale l */
	v3_t Rl, SRl;

	Vx(Rl) = R[0] * Vx(l) + R[1] * Vy(l) + R[2] * Vz(l);
	Vy(Rl) = R[3] * Vx(l) + R[4] * Vy(l) + R[5] * Vz(l);
	Vz(Rl) = R[6] * Vx(l) + R[7] * Vy(l) + R[8] * Vz(l);

	SRl = v3_scale(*scale, Rl);
	
	resid = v3_sub(r, SRl);
	RMS_sum += v3_magsq(resid);
    }
    
    return sqrt(RMS_sum / n);
}

/* Align two sets of points with a 3D rotation */
double align_3D_rotation(int n, v3_t *r_pts, v3_t *l_pts, double *R)
{
    double A[9];
    double U[9], S[3], VT[9], RT[9];
    int i;
    double error;

    for (i = 0; i < 9; i++)
	A[i] = 0.0;

    for (i = 0; i < n; i++) {
	double tensor[9];
	matrix_product(1, 3, 3, 1, l_pts[i].p, r_pts[i].p, tensor);
	matrix_sum(3, 3, 3, 3, A, tensor, A);
    }

    dgesvd_driver(3, 3, A, U, S, VT);

    matrix_product(3, 3, 3, 3, U, VT, RT);
    matrix_transpose(3, 3, RT, R);

    /* Compute error */
    error = 0.0;
    for (i = 0; i < n; i++) {
	double rot[3];
	double diff[3];
	double dist;
	matrix_product(3, 3, 3, 1, R, l_pts[i].p, rot);
	matrix_diff(3, 1, 3, 1, rot, r_pts[i].p, diff);
	dist = matrix_norm(3, 1, diff);
	error += dist;
    }

    return sqrt(error / n);
}

/* Align two sets of points with a 3D rotation */
int align_3D_rotation_ransac(int n, v3_t *r_pts, v3_t *l_pts, 
			     int num_ransac_rounds, double ransac_thresh,
			     double *R)
{
    int round;    
    const int min_support = 3;
    v3_t *l_inliers, *r_inliers;
    int num_inliers, max_inliers = 0;
    double Rbest[9];
    int i;

    if (n < 3) {
	printf("[align_3D_rotation_ransac] Error: need at least 3 points!\n");
	return 0;
    }

    l_inliers = (v3_t *) malloc(sizeof(v3_t) * n);
    r_inliers = (v3_t *) malloc(sizeof(v3_t) * n);

    for (round = 0; round < num_ransac_rounds; round++) {
	int support[min_support];
	int i, j;
	v3_t r_pts_small[min_support], l_pts_small[min_support];
	double Rtmp[9];
	
	for (i = 0; i < min_support; i++) {
	    /* Select an index from 0 to n-1 */
	    int idx, reselect;
	    do {
		reselect = 0;
		idx = rand() % n;
		for (j = 0; j < i; j++) {
		    if (support[j] == idx) {
			reselect = 1;
			break;
		    }
		}
	    } while (reselect);

	    support[i] = idx;
	    r_pts_small[i] = r_pts[idx];
	    l_pts_small[i] = l_pts[idx];
	}

	align_3D_rotation(min_support, r_pts_small, l_pts_small, Rtmp);

	/* Count inliers */
	num_inliers = 0;
	for (i = 0; i < n; i++) {
	    double rot[3];
	    double diff[3];
	    double dist;
	    matrix_product(3, 3, 3, 1, Rtmp, l_pts[i].p, rot);
	    matrix_diff(3, 1, 3, 1, rot, r_pts[i].p, diff);
	    dist = matrix_norm(3, 1, diff);
	    
	    if (dist < ransac_thresh) {
		num_inliers++;
	    }

	    if (num_inliers > max_inliers) {
		num_inliers = max_inliers;
		memcpy(Rbest, Rtmp, sizeof(double) * 9);
	    }
	}
    }

    /* Reestimate using all inliers */
    num_inliers = 0;
    for (i = 0; i < n; i++) {
	double rot[3];
	double diff[3];
	double dist;
	matrix_product(3, 3, 3, 1, Rbest, l_pts[i].p, rot);
	matrix_diff(3, 1, 3, 1, rot, r_pts[i].p, diff);
	dist = matrix_norm(3, 1, diff);
	    
	if (dist < ransac_thresh) {
	    r_inliers[num_inliers] = r_pts[i];
	    l_inliers[num_inliers] = l_pts[i];
	    num_inliers++;
	}
    }

    align_3D_rotation(num_inliers, r_inliers, l_inliers, R);

    free(r_inliers);
    free(l_inliers);

    return num_inliers;
}
