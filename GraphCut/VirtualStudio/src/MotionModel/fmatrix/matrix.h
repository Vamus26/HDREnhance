/* matrix.h */
/* Various linear algebra routines */

#ifndef __matrix_h__
#define __matrix_h__

#ifdef __cplusplus
extern "C" {
#endif
   
/* Fill a given matrix with an n x n identity matrix */
void matrix_ident(int n, double *A);

/* Transpose the m x n matrix A and put the result in the n x m matrix AT */
void matrix_transpose(int m, int n, double *A, double *AT);

/* Compute the matrix product R = AB */
void matrix_product(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute the power of a matrix */
void matrix_power(int n, double *A, int pow, double *R);

/* Compute the matrix product R = A B^T */
void matrix_transpose_product2(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute the matrix sum R = A + B */
void matrix_sum(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute the matrix difference R = A - B */
void matrix_diff(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute the determinant of a 3x3 matrix */
double matrix_determinant3(double *A);

/* Compute the matrix product R = A^T B */
void matrix_transpose_product(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Invert the n-by-n matrix A, storing the result in Ainv */
void matrix_invert(int n, double *A, double *Ainv);

/* Get the norm of the matrix */
double matrix_norm(int m, int n, double *A);

/* Get the [squared] norm of the matrix */
double matrix_normsq(int m, int n, double *A);

/* Scale a matrix by a scalar */
void matrix_scale(int m, int n, double *A, double s, double *R);

/* Print the given m x n matrix */
void matrix_print(int m, int n, double *A);

/* Read a matrix from a file */
void matrix_read_file(int m, int n, double *matrix, char *fname);

/* Write a matrix to a file */
void matrix_write_file(int m, int n, double *matrix, char *fname);

/* Return the product x**T A x */
double matrix_double_product(int n, double *A, double *x);

/* Compute the cross product of two 3 x 1 vectors */
void matrix_cross(const double *u, const double *v, double *w);
void matrix_cross4(const double *u, const double *v, const double *w, 
		   double *x);
    
/* Create the 3x3 cross product matrix from a 3-vector */
void matrix_cross_matrix(double *v, double *v_cross);

/* Convert a rotation matrix to axis and angle representation */
void matrix_to_axis_angle(double *R, double *axis, double *angle);
void axis_angle_to_matrix(double *axis, double angle, double *R);
void axis_angle_to_matrix4(double *axis, double angle, double *R);

/* Convert a matrix to a normalize quaternion */
void matrix_to_quaternion(double *R, double *q);
/* Convert a normalized quaternion to a matrix */
void quaternion_to_matrix(double *q, double *R);
    
/* Decompose a square matrix into an orthogonal matrix and a symmetric
 * positive semidefinite matrix */
void matrix_polar_decomposition(int n, double *A, double *Q, double *S);
    
/* Driver for the minpack function lmdif, which uses
 * Levenberg-Marquardt for non-linear least squares minimization */
void lmdif_driver(void *fcn, int m, int n, double *xvec, double tol);
void lmdif_driver2(void *fcn, int m, int n, double *xvec, double tol);

/* Driver for the lapack function dgelss, which finds x to minimize
 * norm(b - A * x) */
void dgelss_driver(double *A, double *b, double *x, int m, int n, int nrhs);
void dgelsy_driver(double *A, double *b, double *x, int m, int n, int nrhs);

/* Version of above where matrix is already in column-major order */
void dgelsy_driver_transpose(double *A, double *b, double *x, 
			     int m, int n, int nrhs);

/* Solve an n x n system */
void dgesv_driver(int n, double *A, double *b, double *x);
    
/* n: the order of matrix A
 * A: matrix for which the eigenvectors/values are to be computed
 * evec: output array containing the eigenvectors
 * eval: output array containing the eigenvalues
 *
 * Note: Assumes the results are real! */
void dgeev_driver(int n, double *A, double *evec, double *eval);

/* Compute singular value decomposition of an m x n matrix A */
void dgesvd_driver(int m, int n, double *A, double *U, double *S, double *VT);

/* Compute Cholesky decomposition of an nxn matrix */
void dpotrf_driver(int n, double *A, double *U);

/* Compute a QR factorization of an m by n matrix A */
void dgeqrf_driver(int m, int n, double *A, double *Q, double *R);

/* Compute an RQ factorization of an m by n matrix A */
void dgerqf_driver(int m, int n, double *A, double *R, double *Q);

/* Find the unit vector that minimizes ||Ax|| */
void matrix_minimum_unit_norm_solution(int m, int n, double *A, double *x);
    
#ifdef __cplusplus
}
#endif

#endif /* __matrix_h__ */
