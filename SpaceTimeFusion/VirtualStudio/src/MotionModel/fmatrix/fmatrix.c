/* fmatrix_pro.c */
/* Routines for estimating the fundamental matrix of a pair of images */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>

#include "colin_defines.h"
#include "fmatrix.h"
#include "matrix.h"
#include "vector.h"

/* Compute the epipoles of an F-matrix */
void fmatrix_compute_epipoles(double *F, double *e1, double *e2) {
    double Fout[9];
    double U[9], VT[9];

    /* Use SVD to compute the nearest rank 2 matrix */
    closest_rank2_matrix(F, Fout, U, VT);

    /* The last column of U spans the nullspace of F, so it is the
     * epipole in image A.  The last column of V spans the nullspace
     * of F^T, so is the epipole in image B */

    e1[0] = U[2];
    e1[1] = U[5];
    e1[2] = U[8];
    
    e2[0] = VT[6];
    e2[1] = VT[7];
    e2[2] = VT[8];
}

double fmatrix_compute_residual(double *F, v3_t r, v3_t l) {
    double Fl[3], Fr[3], pt;    

    Fl[0] = F[0] * Vx(l) + F[1] * Vy(l) + F[2] * Vz(l);
    Fl[1] = F[3] * Vx(l) + F[4] * Vy(l) + F[5] * Vz(l);
    Fl[2] = F[6] * Vx(l) + F[7] * Vy(l) + F[8] * Vz(l);

    Fr[0] = F[0] * Vx(r) + F[3] * Vy(r) + F[6] * Vz(r);
    Fr[1] = F[1] * Vx(r) + F[4] * Vy(r) + F[7] * Vz(r);
    Fr[2] = F[2] * Vx(r) + F[5] * Vy(r) + F[8] * Vz(r);

    pt = Vx(r) * Fl[0] + Vy(r) * Fl[1] + Vz(r) * Fl[2];

    return 
	(1.0 / (Fl[0] * Fl[0] + Fl[1] * Fl[1]) +
	 1.0 / (Fr[0] * Fr[0] + Fr[1] * Fr[1])) *
	(pt * pt);
}

int double_compare(const void *a, const void *b) {
    double *ad = (double *) a;
    double *bd = (double *) b;
    
    if (*ad < *bd)
	return -1;
    else if (*ad == *bd)
	return 0;
    else
	return 1;
}

static v3_t *global_ins = NULL;
static v3_t *global_outs = NULL;
static int global_num_matches = 0;
static double global_scale;

void fmatrix_residuals(int *m, int *n, double *x, double *fvec, int *iflag) {
    int i;
    double sum = 0.0;

    double F[9], F2[9], U[9], VT[9];
    memcpy(F, x, sizeof(double) * 8);
    F[8] = global_scale;

    closest_rank2_matrix(F, F2, U, VT);

    if (global_num_matches != (*m)) {
	printf("Error: number of matches don't match!\n");
    }

    for (i = 0; i < global_num_matches; i++) {
	fvec[i] = sqrt(fmatrix_compute_residual(F2, global_outs[i], 
						global_ins[i]));
	if (*iflag == 0) {
	    sum += fvec[i];
	}
    }
}

/* Find the closest rank 2 matrix to the given 3x3 matrix */
void closest_rank2_matrix(double *Fin, double *Fout, double *U, double *VT) {
    double S[3], sigma[9], F_rank2[9], tmp[9];

    dgesvd_driver(3, 3, Fin, U, S, VT);

    sigma[0] = S[0];  sigma[1] =  0.0;  sigma[2] =  0.0;
    sigma[3] =  0.0;  sigma[4] = S[1];  sigma[5] =  0.0;
    sigma[6] =  0.0;  sigma[7] =  0.0;  sigma[8] =  0.0;

    matrix_product(3, 3, 3, 3, U, sigma, tmp);
    matrix_product(3, 3, 3, 3, tmp, VT, F_rank2);

    memcpy(Fout, F_rank2, sizeof(double) * 9);
}

/* Refine an F-matrix estimate using LM */
void refine_fmatrix_nonlinear_matches(int num_pts, v3_t *r_pts, v3_t *l_pts, 
				      double *F0, double *Fout)
{
    global_ins = l_pts;
    global_outs = r_pts;
    global_num_matches = num_pts;
    global_scale = F0[8];
    
    memcpy(Fout, F0, sizeof(double) * 9);

    lmdif_driver2(fmatrix_residuals, num_pts, 8, Fout, 1.0e-12);

    Fout[8] = global_scale;

    global_ins = global_outs = NULL;
    global_num_matches = 0;    
}

/* Use linear least-squares to estimate the fundamantal matrix */
void estimate_fmatrix(int num_pts, v3_t *r_pts, v3_t *l_pts, double *Fout, int nonLinearRefine)
{
	int i;
	v3_t r_c, l_c;
	double r_dist, l_dist, r_scale, l_scale;

	v3_t *r_pts_new, *l_pts_new;

	double *A, *b, X[8], F[9], H[9], H_p[9], tmp[9], F_new[9];
	double U[9], VT[9];

	/* Check that there are enough point correspondences */
	if (num_pts < 8)
	{
		printf("[estimate_fmatrix_linear] Insufficient correspondences (need at least 8, given only %d)\n", num_pts);
		return;
	}

	/* First, compute the centroid of both sets of points */
	r_c = v3_new(0.0, 0.0, 0.0);
	l_c = v3_new(0.0, 0.0, 0.0);

	for (i = 0; i < num_pts; i++) {
		r_c = v3_add(r_c, r_pts[i]);
		l_c = v3_add(l_c, l_pts[i]);
	}

	r_c = v3_scale(1.0 / num_pts, r_c);
	l_c = v3_scale(1.0 / num_pts, l_c);


	/* Compute the average distance from each point to the centroid */
	r_dist = l_dist = 0;

	for (i = 0; i < num_pts; i++) {
		r_dist += v3_mag(v3_sub(r_c, r_pts[i]));
		l_dist += v3_mag(v3_sub(l_c, l_pts[i]));
	}

	r_dist /= num_pts;
	l_dist /= num_pts;

	r_dist /= sqrt(2.0);
	l_dist /= sqrt(2.0);

	r_scale = 1.0 / r_dist;
	l_scale = 1.0 / l_dist;


	/* Normalize the points with an affine transform */
	r_pts_new = (v3_t *)malloc(sizeof(v3_t) * num_pts);
	l_pts_new = (v3_t *)malloc(sizeof(v3_t) * num_pts);


	for (i = 0; i < num_pts; i++) {
		r_pts_new[i] = v3_scale(r_scale, v3_sub(r_pts[i], r_c));
		l_pts_new[i] = v3_scale(l_scale, v3_sub(l_pts[i], l_c));

		Vz(r_pts_new[i]) = 1.0;
		Vz(l_pts_new[i]) = 1.0;
	}


	/* Fill in the rows of the matrix A */
	A = (double *)malloc(sizeof(double) * 8 * num_pts);

	for (i = 0; i < num_pts; i++) {
		double u = Vx(l_pts_new[i]);
		double v = Vy(l_pts_new[i]);
		double u_p = Vx(r_pts_new[i]);
		double v_p = Vy(r_pts_new[i]);

		A[i * 8 + 0] = u * u_p;
		A[i * 8 + 1] = v * u_p;
		A[i * 8 + 2] = u_p;
		A[i * 8 + 3] = u * v_p;
		A[i * 8 + 4] = v * v_p;
		A[i * 8 + 5] = v_p;
		A[i * 8 + 6] = u;
		A[i * 8 + 7] = v;
	}


	/* Fill in the vector b */
	b = (double *)malloc(sizeof(double) * num_pts);

	for (i = 0; i < num_pts; i++) {
		b[i] = -1.0;
	}

	/* Solve for the least-squares solution to the F-matrix */
	dgelsy_driver(A, b, X, num_pts, 8, 1);

	memcpy(F, X, sizeof(double) * 8);
	F[8] = 1.0;

	if(nonLinearRefine)
	{
		double F_nonLin[9];
		refine_fmatrix_nonlinear_matches(num_pts, r_pts_new, l_pts_new, F, F_nonLin);
		memcpy(F, F_nonLin, sizeof(double) * 9);
	}

	/* Use SVD to compute the nearest rank 2 matrix */
	closest_rank2_matrix(F, F_new, U, VT);

	/* Un-normalize */
	H[0] = l_scale;  H[1] =     0.0;  H[2] = -l_scale * Vx(l_c);
	H[3] =     0.0;  H[4] = l_scale;  H[5] = -l_scale * Vy(l_c);
	H[6] =     0.0;  H[7] =     0.0;  H[8] =                1.0;

	H_p[0] = r_scale;  H_p[3] =     0.0;  H_p[6] = -r_scale * Vx(r_c);
	H_p[1] =     0.0;  H_p[4] = r_scale;  H_p[7] = -r_scale * Vy(r_c);
	H_p[2] =     0.0;  H_p[5] =     0.0;  H_p[8] =                1.0;

	matrix_product(3, 3, 3, 3, H_p, F_new, tmp);
	matrix_product(3, 3, 3, 3, tmp, H, Fout);

	/* Cleanup */
	free(A);
	free(b);

	free(r_pts_new);
	free(l_pts_new);
}


/* Normalize an n-dimensional vector so that the last coordinate is 1 */
void homogenize(int n, double *v) {
    if (v[n-1] == 0.0) {
	return;
    } else {
	int i;
	
	for (i = 0; i < n; i++) {
	    v[i] /= v[n-1];
	}
    }
}

/* Estimate the essential matrix from an F-matrix, assuming 
 * same focal lengths */
void estimate_essential_same_focal_lengths(double *F, double *alpha, double *E)
{
    int i;
    double best_ratio = DBL_MAX;
    double best_guess = 100.0;
    double bf, bfsq;

#define MIN_FOCAL_LENGTH 100.0
#define FOCAL_LENGTH_STEP 10.0
    for (i = 0; i < 1000; i++) {
	double U[9], S[3], VT[9];
	double s1, s2, ratio;
	double f = FOCAL_LENGTH_STEP * i + MIN_FOCAL_LENGTH;
	double Etest[9] = 
	    { f * f * F[0], f * f * F[1], f * F[2],
	      f * f * F[3], f * f * F[4], f * F[5],
	      f * F[6],     f * F[7],     F[8] };

	/* Use SVD */
	dgesvd_driver(3, 3, Etest, U, S, VT);

	s1 = S[0];
	s2 = S[1];
	
	ratio = s1 / s2;
	
	// printf("[eesfl] %0.3f => %0.3f\n", f, ratio);

	if (fabs(ratio - 1.0) < fabs(best_ratio - 1.0)) {
	    best_ratio = ratio;
	    best_guess = f;
	}
    }

    bf = best_guess;
    bfsq = bf * bf;

    E[0] = bfsq * F[0];  E[1] = bfsq * F[1];  E[2] = bf * F[2];
    E[3] = bfsq * F[3];  E[4] = bfsq * F[4];  E[5] = bf * F[5];
    E[6] = bf * F[6];    E[7] = bf * F[7];    E[8] = F[8];

    *alpha = best_guess;

    printf("[eesfl] best: %0.3f ==> %0.3f\n", best_guess, best_ratio);
}

/* Estimate the essential matrix from an F-matrix, assuming 
 * different focal lengths */
void estimate_essential_different_focal_lengths(double *F, 
						double *calib1, double *calib2,
						double *E)
{
    int i, j;
    double best_ratio = DBL_MAX;
    double best_guess1 = 100.0, best_guess2 = 100.0;
    double bf1, bf2, bfm;

#define MIN_FOCAL_LENGTH 100.0
#define FOCAL_LENGTH_STEP 10.0
    for (i = 0; i < 400; i++) {
	double f1 = FOCAL_LENGTH_STEP * i + MIN_FOCAL_LENGTH;

	for (j = 0; j < 400; j++) {
	    double f2 = FOCAL_LENGTH_STEP * j + MIN_FOCAL_LENGTH;
	    double fm = f1 * f2;

	    double U[9], S[3], VT[9];
	    double s1, s2, ratio;

	    double Etest[9] = 
		{ fm * F[0], fm * F[1], f1 * F[2],
		  fm * F[3], fm * F[4], f1 * F[5],
		  f2 * F[6], f2 * F[7], F[8] };

	    /* Use SVD */
	    dgesvd_driver(3, 3, Etest, U, S, VT);

	    s1 = S[0];
	    s2 = S[1];
	
	    ratio = s1 / s2;
	
	    // printf("[eesfl] %0.3f => %0.3f\n", f, ratio);

	    if (fabs(ratio - 1.0) < fabs(best_ratio - 1.0)) {
		best_ratio = ratio;
		best_guess1 = f1;
		best_guess2 = f2;
	    }
	}
    }
    
    bf1 = best_guess1;
    bf2 = best_guess2;
    bfm = bf1 * bf2;

    E[0] = bfm * F[0];  E[1] = bfm * F[1];  E[2] = bf1 * F[2];
    E[3] = bfm * F[3];  E[4] = bfm * F[4];  E[5] = bf1 * F[5];
    E[6] = bf2 * F[6];  E[7] = bf2 * F[7];  E[8] = F[8];

    calib1[0] = bf1;  calib1[1] = 0.0;  calib1[2] = 0.0;
    calib1[3] = 0.0;  calib1[4] = bf1;  calib1[5] = 0.0;
    calib1[6] = 0.0;  calib1[7] = 0.0;  calib1[8] = 1.0;

    calib2[0] = bf2;  calib2[1] = 0.0;  calib2[2] = 0.0;
    calib2[3] = 0.0;  calib2[4] = bf2;  calib2[5] = 0.0;
    calib2[6] = 0.0;  calib2[7] = 0.0;  calib2[8] = 1.0;

    printf("[eedfl] best: %0.3f, %0.3f ==> %0.3f\n", 
	   best_guess1, best_guess2, best_ratio);
}

/* Compute an F-matrix from two sets of camera parameters */
void fmatrix_from_parameters(double *i0, double *R0, double *t0, 
			     double *i1, double *R1, double *t1, double *F) {
    
    /* Apply the rotation and translation so that the first camera is
     * at the origin, pointing in the canonical direction */
    
    double R1_new[9];
    double t1_new[3];
    double t1_cross[9];
    double E[9];
    double i0_inv[9], i1_inv[9], tmp[9];

    matrix_transpose_product(3, 3, 3, 3, R0, R1, R1_new);
    matrix_diff(3, 1, 3, 1, t1, t0, t1_new);

    matrix_cross_matrix(t1_new, t1_cross);

    /* Essential matrix */
    matrix_product(3, 3, 3, 3, t1_cross, R1_new, E);

    /* Fundamental matrix */
    matrix_invert(3, i0, i0_inv);
    matrix_invert(3, i1, i1_inv);

    matrix_transpose_product(3, 3, 3, 3, i1_inv, E, tmp);
    matrix_product(3, 3, 3, 3, tmp, i0_inv, F);
}


#if 0
/* Refine an F-matrix estimate using LM */
void refine_fmatrix_nonlinear_matches(int num_pts, v3_t *r_pts, v3_t *l_pts, 
				      double *F0, double *Fout)
{
    double Ftmp[9];
    double U[9], VT[9];

    global_ins = l_pts;
    global_outs = r_pts;
    global_num_matches = num_pts;
    global_scale = F0[8];
    
    memcpy(Ftmp, F0, sizeof(double) * 9);

    lmdif_driver2(fmatrix_residuals, num_pts, 8, Ftmp, 1.0e-12);

    Ftmp[8] = global_scale;
    //matrix_print(3, 3, Ftmp);
    closest_rank2_matrix(Ftmp, Fout, U, VT);
    //matrix_print(3, 3, Fout);

    global_ins = global_outs = NULL;
    global_num_matches = 0;    
}

void estimate_fmatrix_linear(int num_pts, v3_t *r_pts, v3_t *l_pts, 
			     double *Fout, double *e1, double *e2)
{
	int i;
	v3_t r_c, l_c;
	double r_dist, l_dist, r_scale, l_scale;

	v3_t *r_pts_new, *l_pts_new;

	double *A, *b, X[8], F[9], H[9], H_p[9], tmp[9], F_new[9];
	double U[9], VT[9];

	if (num_pts < 8)
	{
		printf("[estimate_fmatrix_linear] Insufficient correspondences (need at least 8, given only %d)\n", num_pts);
		return;
	}

	r_c = v3_new(0.0, 0.0, 0.0);
	l_c = v3_new(0.0, 0.0, 0.0);

	for (i = 0; i < num_pts; i++) {
		r_c = v3_add(r_c, r_pts[i]);
		l_c = v3_add(l_c, l_pts[i]);
	}

	r_c = v3_scale(1.0 / num_pts, r_c);
	l_c = v3_scale(1.0 / num_pts, l_c);


	r_dist = l_dist = 0;

	for (i = 0; i < num_pts; i++) {
		r_dist += v3_mag(v3_sub(r_c, r_pts[i]));
		l_dist += v3_mag(v3_sub(l_c, l_pts[i]));
	}

	r_dist /= num_pts;
	l_dist /= num_pts;

	r_dist /= sqrt(2.0);
	l_dist /= sqrt(2.0);

	r_scale = 1.0 / r_dist;
	l_scale = 1.0 / l_dist;


	r_pts_new = (v3_t *)malloc(sizeof(v3_t) * num_pts);
	l_pts_new = (v3_t *)malloc(sizeof(v3_t) * num_pts);


	for (i = 0; i < num_pts; i++) {
		r_pts_new[i] = v3_scale(r_scale, v3_sub(r_pts[i], r_c));
		l_pts_new[i] = v3_scale(l_scale, v3_sub(l_pts[i], l_c));

		Vz(r_pts_new[i]) = 1.0;
		Vz(l_pts_new[i]) = 1.0;
	}


	A = (double *)malloc(sizeof(double) * 8 * num_pts);

	for (i = 0; i < num_pts; i++) {
		double u = Vx(l_pts_new[i]);
		double v = Vy(l_pts_new[i]);
		double u_p = Vx(r_pts_new[i]);
		double v_p = Vy(r_pts_new[i]);

		A[i * 8 + 0] = u * u_p;
		A[i * 8 + 1] = v * u_p;
		A[i * 8 + 2] = u_p;
		A[i * 8 + 3] = u * v_p;
		A[i * 8 + 4] = v * v_p;
		A[i * 8 + 5] = v_p;
		A[i * 8 + 6] = u;
		A[i * 8 + 7] = v;
	}


	b = (double *)malloc(sizeof(double) * num_pts);

	for (i = 0; i < num_pts; i++) {
		b[i] = -1.0;
	}


	dgelsy_driver(A, b, X, num_pts, 8, 1);


	H[0] = l_scale;  H[1] =     0.0;  H[2] = -l_scale * Vx(l_c);
	H[3] =     0.0;  H[4] = l_scale;  H[5] = -l_scale * Vy(l_c);
	H[6] =     0.0;  H[7] =     0.0;  H[8] =                1.0;

	H_p[0] = r_scale;  H_p[3] =     0.0;  H_p[6] = -r_scale * Vx(r_c);
	H_p[1] =     0.0;  H_p[4] = r_scale;  H_p[7] = -r_scale * Vy(r_c);
	H_p[2] =     0.0;  H_p[5] =     0.0;  H_p[8] =                1.0;

	memcpy(F, X, sizeof(double) * 8);
	F[8] = 1.0;

	matrix_product(3, 3, 3, 3, H_p, F, tmp);
	matrix_product(3, 3, 3, 3, tmp, H, F_new);

	closest_rank2_matrix(F_new, Fout, U, VT);


	e1[0] = U[2];
	e1[1] = U[5];
	e1[2] = U[8];

	e2[0] = VT[6];
	e2[1] = VT[7];
	e2[2] = VT[8];


	free(A);
	free(b);

	free(r_pts_new);
	free(l_pts_new);
}
#endif