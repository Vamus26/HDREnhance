#include <stdlib.h>
#include <stdio.h>

#include <float.h>
#include <math.h>
#include <string.h>

#include "lapack.h"
#include "minpack.h"

#include "colin_defines.h"

/* Fill a given matrix with an n x n identity matrix */
void matrix_ident(int n, double *A) {
    int i, j;

    for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++) {
	    if (i == j)
		A[i * n + j] = 1.0;
	    else
		A[i * n + j] = 0.0;
	}
    }
}

/* Transpose the m x n matrix A and put the result in the n x m matrix AT */
void matrix_transpose(int m, int n, double *A, double *AT) {
    int i, j;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            AT[j * m + i] = A[i * n + j];
}

/* Compute the matrix product R = AB */
void matrix_product(int Am, int An, int Bm, int Bn, double *A, double *B, double *R) {
    int i, j, k;
    int r = Am;
    int c = Bn;
    int m = An;
    
    if (An != Bm) {
        printf("[matrix_product] Error: the number of columns of A and the "
               "number of rows of B must be equal\n");
        return;
    }
    
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            R[i * c + j] = 0.0;
            for (k = 0; k < m; k++) {
                R[i * c + j] += A[i * An + k] * B[k * Bn + j];
            }
        }
    }
}

/* Compute the power of a matrix */
void matrix_power(int n, double *A, int pow, double *R)
{
    int i;
    
    /* Slow way */
    double *curr = (double *) malloc(sizeof(double) * n * n);
    double *tmp = (double *) malloc(sizeof(double) * n * n);

    matrix_ident(n, curr);
    for (i = 0; i < pow; i++) {
	matrix_product(n, n, n, n, curr, A, tmp);
	memcpy(curr, tmp, sizeof(double) * n * n);
    }

    memcpy(R, curr, sizeof(double) * n * n);

    free(curr);
    free(tmp);
}

/* Compute the matrix sum R = A + B */
void matrix_sum(int Am, int An, int Bm, int Bn, double *A, double *B, double *R) {
    int r = Am;
    int c = An;
    int n = r * c, i;
    
    if (Am != Bm || An != Bn) {
	printf("[matrix_sum] Error: mismatched dimensions\n");
	return;
    }

    for (i = 0; i < n; i++) {
	R[i] = A[i] + B[i];
    }
}

/* Compute the matrix difference R = A - B */
void matrix_diff(int Am, int An, int Bm, int Bn, double *A, double *B, double *R) {
    int r = Am;
    int c = An;
    int n = r * c, i;
    
    if (Am != Bm || An != Bn) {
	printf("[matrix_sum] Error: mismatched dimensions\n");
	return;
    }

    for (i = 0; i < n; i++) {
	R[i] = A[i] - B[i];
    }    
}

/* Compute the matrix product R = A^T B */
void matrix_transpose_product(int Am, int An, int Bm, int Bn, double *A, double *B, double *R) {
    int i, j, k;
    int r = An;
    int c = Bn;
    int m = Am;
    
    if (Am != Bm) {
        printf("Error: the number of rows of A and the "
               "number of rows of B must be equal\n");
        return;
    }
    
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            R[i * c + j] = 0.0;
            for (k = 0; k < m; k++) {
                R[i * c + j] += A[k * An + i] * B[k * Bn + j];
            }
        }
    }
}

/* Compute the matrix product R = A B^T */
void matrix_transpose_product2(int Am, int An, int Bm, int Bn, double *A, double *B, double *R) {
    int i, j, k;
    int r = Am;
    int c = Bm;
    int m = An;
    
    if (An != Bn) {
        printf("Error: the number of columns of A and the "
               "number of columns of B must be equal\n");
        return;
    }
    
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            R[i * c + j] = 0.0;
            for (k = 0; k < m; k++) {
                R[i * c + j] += A[i * An + k] * B[j * Bn + k];
            }
        }
    }
}

/* Return the product x**T A x */
double matrix_double_product(int n, double *A, double *x) {
    double *tmp = malloc(sizeof(double) * n);
    double result;
    
    matrix_product(n, n, n, 1, A, x, tmp);
    matrix_product(1, n, n, 1, x, tmp, &result);

    free(tmp);

    return result;
}

/* n: the order of matrix A
 * A: matrix for which the eigenvectors/values are to be computed
 * evec: output array containing the eigenvectors
 * eval: output array containing the eigenvalues
 *
 * Note: Assumes the results are real! */

void dgeev_driver(int n, double *A, double *evec, double *eval) {
    char jobvl = 'N';  /* Don't compute left eigenvectors */
    char jobvr = 'V';  /* Do compute right eigenvectors */
    int lda = n;
    double *Atmp = malloc(sizeof(double) * n * n);
    double *wr = malloc(sizeof(double) * n);
    double *wi = malloc(sizeof(double) * n);
    double *vl = NULL;
    int ldvl = 1;
    double *vr = malloc(sizeof(double) * n * n);
    int ldvr = n;
    int lwork;
    double *work, work_query[1];
    int info;

    int i, j;

    /* Transpose the matrix for FORTRAN */
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            Atmp[j * n + i] = A[i * n + j];

    /* Query dgeev for the optimal value of lwork */
    lwork = -1;
    dgeev_(&jobvl, &jobvr, &n, Atmp, &lda, wr, wi, vl, &ldvl, vr, &ldvr, work_query, &lwork, &info);
    lwork = (int) work_query[0];
    work = malloc(sizeof(double) * lwork);

    /* Make the call to dgeev */
    dgeev_(&jobvl, &jobvr, &n, Atmp, &lda, wr, wi, vl, &ldvl, vr, &ldvr, work, &lwork, &info);
    
    if (info < 0)
        printf("Error in call to dgeev (argument %d was invalid\n", -info);
    else if (info > 0)
        printf("Error: not all eigenvalues have converged\n");
    
    /* Check that all eigenvalues are real */
    for (i = 0; i < n; i++) {
        if (wi[i] != 0.0)
            printf("[dgeev] Eigenvalue has non-zero imaginary part\n");
        eval[i] = wr[i];

        for (j = 0; j < n; j++)
            evec[i * n + j] = vr[i * n + j];
    }

    /* Clean up */
    free(work);
    free(Atmp);
    free(wr);
    free(wi);
    free(vr);
}

void lmdif_driver(void *fcn, int m, int n, double *xvec, double tol) {
    int info;
    int lwa = m * n + 5 * n + m;
    int *iwa;
    double *fvec, *wa;

    if (n > m) {
        printf("Error: lmdif called with n > m\n");
        return;
    }

    iwa = (int *)malloc(sizeof(int) * n);
    fvec = (double *)malloc(sizeof(double) * m);
    wa = (double *)malloc(sizeof(double) * lwa);

	lmdif1_(fcn, &m, &n, xvec, fvec, &tol, &info, iwa, wa, &lwa);

#if 0
    switch (info) {
        case 0:
            printf("Improper input parameters\n");
            break;
        case 1:
            printf("Sum of squares tolerance reached\n");
            break;
        case 2:
            printf("x is within tolerance\n");
            break;
        case 3:
            printf("Sum of squares and x are within tolerance\n");
            break;
        case 4:
            printf("fvec orthogonal\n");
            break;
        case 5:
            printf("max function calls made\n");
            break;
        case 6:
            printf("tolerance is too small (squares)\n");
            break;
        case 7:
            printf("tolerance is too small (x)\n");
            break;
    }
#endif

    free(iwa);
    free(fvec);
    free(wa);
}

void lmdif_driver2(void *fcn, int m, int n, double *xvec, double tol) {
    int info;
    double *fvec;
    double gtol = 0, epsfcn = 0;
    int maxfev = 200 * (n + 1);
    double *diag;
    int mode = 1;
    double factor = 100;
    int nprint = 1;
    int nfev;
    double *fjac;
    int ldfjac = m;
    int *ipvt;
    double *qtf;
    double *wa1, *wa2, *wa3, *wa4;

    if (n > m) {
        printf("Error: lmdif called with n > m\n");
        return;
    }

    fvec = (double *)malloc(sizeof(double) * m);
    diag = (double *)malloc(sizeof(double) * n);
    fjac = (double *)malloc(sizeof(double) * m * n);
    ipvt = (int *)malloc(sizeof(int) * n);
    qtf = (double *)malloc(sizeof(double) * n);
    wa1 = (double *)malloc(sizeof(double) * n);
    wa2 = (double *)malloc(sizeof(double) * n);
    wa3 = (double *)malloc(sizeof(double) * n);
    wa4 = (double *)malloc(sizeof(double) * m);

    lmdif_(fcn, &m, &n, xvec, fvec, &tol, &tol, &gtol, &maxfev, 
           &epsfcn, diag, &mode, &factor, &nprint, &info, &nfev, 
           fjac, &ldfjac, ipvt, qtf, wa1, wa2, wa3, wa4);

#if 0
    switch (info) {
        case 0:
            printf("Improper input parameters\n");
            break;
        case 1:
            printf("Sum of squares tolerance reached\n");
            break;
        case 2:
            printf("x is within tolerance\n");
            break;
        case 3:
            printf("Sum of squares and x are within tolerance\n");
            break;
        case 4:
            printf("fvec orthogonal\n");
            break;
        case 5:
            printf("max function calls made\n");
            break;
        case 6:
            printf("tolerance is too small (squares)\n");
            break;
        case 7:
            printf("tolerance is too small (x)\n");
            break;
    default:
	printf("???\n");
    }
#endif

    /* Clean up */
    free(fvec);
    free(diag);
    free(fjac);
    free(ipvt);
    free(qtf);
    free(wa1);
    free(wa2);
    free(wa3);
    free(wa4);
}

void dgelss_driver(double *A, double *b, double *x, int m, int n, int nrhs) {
    if (m < n) {
        printf("Error: driver now only works when m >= n\n");
        return;
    } else {
        double *Atmp = malloc(sizeof(double) * m * n);
        double *btmp = malloc(sizeof(double) * m * nrhs);
        int lda = m;
        int ldb = m;
        double *s = malloc(sizeof(double) * n); /* Output array */
        double rcond = -1.0;
        int rank; /* Output */
        int lwork = 16 * (3 * MIN(m, n) + MAX(MAX(2 * MIN(m, n), MAX(m, n)), nrhs));
        double *work = malloc(sizeof(double) * lwork);
        int info;

        int i, j;

        /* Go from row- to column-major */
        for (i = 0; i < m; i++)
            for (j = 0; j < n; j++)
                Atmp[j * m + i] = A[i * n + j];
        
        for (i = 0; i < m; i++)
            for (j = 0; j < nrhs; j++)
                btmp[j * m + i] = b[i * nrhs + j];

        /* Make the FORTRAN call */
        dgelss_(&m, &n, &nrhs, Atmp, &lda, btmp, &ldb, 
                s, &rcond, &rank, work, &lwork, &info);

        /* Go from column- to row-major */
        for (i = 0; i < n; i++)
            for (j = 0; j < nrhs; j++)
                x[i * nrhs + j] = btmp[j * m + i];
        
        free(Atmp);
        free(btmp);
        free(s);
        free(work);
    }
}

void dgelsy_driver(double *A, double *b, double *x, int m, int n, int nrhs) {
    if (m < n) {
        printf("Error: driver now only works when m >= n\n");
        return;
    } else {
        double *Atmp = malloc(sizeof(double) * m * n);
        double *btmp = malloc(sizeof(double) * m * nrhs);
        int lda = m;
        int ldb = m;
        int *jpvt = calloc(sizeof(int), n);
        double rcond = -1.0;
        int rank; /* Output */
        int lwork = -1;
        double *work = malloc(sizeof(double) * 1);
        int info;

        int i, j;

        /* Go from row- to column-major */
        for (i = 0; i < m; i++)
            for (j = 0; j < n; j++)
                Atmp[j * m + i] = A[i * n + j];
        
        for (i = 0; i < m; i++)
            for (j = 0; j < nrhs; j++)
                btmp[j * m + i] = b[i * nrhs + j];

        /* Query to find a good size for the work array */
        dgelsy_(&m, &n, &nrhs, Atmp, &lda, btmp, &ldb, jpvt,
                &rcond, &rank, work, &lwork, &info);

        lwork = (int) work[0];
        /* printf("Work size: %d\n", lwork); */
        free(work);
        work = malloc(sizeof(double) * lwork);

        /* Make the FORTRAN call */
        dgelsy_(&m, &n, &nrhs, Atmp, &lda, btmp, &ldb, jpvt,
                &rcond, &rank, work, &lwork, &info);

        if (info != 0)
            printf("Error in call to dgelsy\n");

        /* Go from column- to row-major */
        for (i = 0; i < n; i++)
            for (j = 0; j < nrhs; j++)
                x[i * nrhs + j] = btmp[j * m + i];
        
        free(Atmp);
        free(btmp);
        free(work);
	free(jpvt);
    }
}

void dgelsy_driver_transpose(double *A, double *b, double *x, 
			     int m, int n, int nrhs) 
{
    if (m < n) {
        printf("Error: driver now only works when m >= n\n");
        return;
    } else {
        double *btmp = malloc(sizeof(double) * m * nrhs);
        int lda = m;
        int ldb = m;
        int *jpvt = calloc(sizeof(int), n);
        double rcond = -1.0;
        int rank; /* Output */
        int lwork = -1;
        double *work = malloc(sizeof(double) * 1);
        int info;

        int i, j;

		memcpy(btmp, b, sizeof(double) * m);

        /* Query to find a good size for the work array */
        dgelsy_(&m, &n, &nrhs, A, &lda, btmp, &ldb, jpvt,
                &rcond, &rank, work, &lwork, &info);

        lwork = (int) work[0];
        /* printf("Work size: %d\n", lwork); */
        free(work);
        work = malloc(sizeof(double) * lwork);

        /* Make the FORTRAN call */
        dgelsy_(&m, &n, &nrhs, A, &lda, btmp, &ldb, jpvt,
                &rcond, &rank, work, &lwork, &info);

        if (info != 0)
            printf("Error in call to dgelsy\n");

        /* Go from column- to row-major */
        for (i = 0; i < n; i++)
            for (j = 0; j < nrhs; j++)
                x[i * nrhs + j] = btmp[j * m + i];
        
        free(btmp);
        free(work);
	free(jpvt);
    }
}

/* Solve an n x n system */
void dgesv_driver(int n, double *A, double *b, double *x) {
    double *Atmp = malloc(sizeof(double) * n * n);
    double *btmp = malloc(sizeof(double) * n);

    int nrhs = 1;
    int lda = n;
    int ldb = n;
    int *ipiv = calloc(sizeof(int), n);
    
    int info;

    int i, j;

    /* Go from row- to column-major */
    for (i = 0; i < n; i++)
	for (j = 0; j < n; j++)
	    Atmp[j * n + i] = A[i * n + j];
        
    for (i = 0; i < n; i++)
	btmp[i] = b[i];

    /* Make the FORTRAN call */
    dgesv_(&n, &nrhs, Atmp, &lda, ipiv, btmp, &ldb, &info);

    if (info != 0)
	printf("Error in call to dgelsy\n");

    /* Go from column- to row-major */
    for (i = 0; i < n; i++)
	x[i] = btmp[i];
        
    free(Atmp);
    free(btmp);
    free(ipiv);
}

void matrix_invert(int n, double *A, double *Ainv) {
    double *At = malloc(sizeof(double) * n * n);
    int m = n;
    int lda = n;
    int info;
    int *ipiv = malloc(sizeof(int) * n);
    int lwork = n * 512;
    int i, j;
    double *work = malloc(sizeof(double) * lwork);

    /* Transpose A info At like FORTRAN likes */
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            At[i * n + j] = A[j * n + i];

    /* Make calls to FORTRAN routines */
    dgetrf_(&m, &n, At, &lda, ipiv, &info);
    dgetri_(&n, At, &lda, ipiv, work, &lwork, &info);

    /* Transpose back into Ainv */
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            Ainv[i * n + j] = At[j * n + i];

    free(At);
    free(ipiv);
    free(work);
}

/* Compute the determinant of a 3x4 matrix */
double matrix_determinant3(double *A)
{
#if 0
    double *Q = (double *) malloc(sizeof(double) * n * n);
    double *R = (double *) malloc(sizeof(double) * n * n);
    dgeqrf_driver(n, n, A, Q, R);
#endif

    /* 0 1 2
     * 3 4 5
     * 6 7 8 */

    return 
	A[0] * (A[4] * A[8] - A[5] * A[7]) -
	A[1] * (A[3] * A[8] - A[5] * A[6]) +
	A[2] * (A[3] * A[7] - A[4] * A[6]);
}

/* Get the norm of the matrix */
double matrix_norm(int m, int n, double *A) {
    double sum = 0.0;
    int i;
    int entries = m * n;

    for (i = 0; i < entries; i++) {
	sum += A[i] * A[i];
    }
    
    return sqrt(sum);
}

/* Get the [squared] norm of the matrix */
double matrix_normsq(int m, int n, double *A) 
{
    double sum = 0.0;
    int i;
    int entries = m * n;

    for (i = 0; i < entries; i++) {
	sum += A[i] * A[i];
    }

    return sum;
}

/* Cross two 3x1 vectors */
void matrix_cross(const double *u, const double *v, double *w) {
    w[0] = u[1] * v[2] - u[2] * v[1];
    w[1] = u[2] * v[0] - u[0] * v[2];
    w[2] = u[0] * v[1] - u[1] * v[0];
}

/* Create the 3x3 cross product matrix from a 3-vector */
void matrix_cross_matrix(double *v, double *v_cross) {
    v_cross[0] = 0.0;   v_cross[1] = -v[2]; v_cross[2] = v[1];
    v_cross[3] = v[2];  v_cross[4] = 0.0;   v_cross[5] = -v[0];
    v_cross[6] = -v[1]; v_cross[7] = v[0];  v_cross[8] = 0.0;
}

/* Cross three 4x1 vectors */
void matrix_cross4(const double *u, const double *v, const double *w, 
		   double *x) 
{
    double sub1[9] = 
	{ u[1], u[2], u[3],
	  v[1], v[2], v[3],
	  w[1], w[2], w[3] };
    	
    double sub2[9] = 
	{ u[0], u[2], u[3],
	  v[0], v[2], v[3],
	  w[0], w[2], w[3] };

    double sub3[9] = 
	{ u[0], u[1], u[3],
	  v[0], v[1], v[3],
	  w[0], w[1], w[3] };

    double sub4[9] = 
	{ u[0], u[1], u[2],
	  v[0], v[1], v[2],
	  w[0], w[1], w[2] };

    double det1 = matrix_determinant3(sub1);
    double det2 = matrix_determinant3(sub2);
    double det3 = matrix_determinant3(sub3);
    double det4 = matrix_determinant3(sub4);

    x[0] = det1;
    x[1] = det2;
    x[2] = det3;
    x[3] = det4;
}

/* Scale a matrix by a scalar */
void matrix_scale(int m, int n, double *A, double s, double *R) {
    int i;
    int entries = m * n;
    
    for (i = 0; i < entries; i++) {
	R[i] = A[i] * s;
    }
}

/* Print the given n x m matrix */
void matrix_print(int m, int n, double *A) {
    int i, j;
    
    for (i = 0; i < m; i++) {
        printf(" { ");
        for (j = 0; j < n; j++) {
            printf(" %0.6e ", A[i * n + j]);
        }
        printf(" } \n");
    }
}

/* Read a matrix from a file */
void matrix_read_file(int m, int n, double *matrix, char *fname) {
    FILE *f = fopen(fname, "r");
    int i;

    if (f == NULL) {
	printf("[matrix_read_file] Error reading matrix %s\n", fname);
	return;
    }

    for (i = 0; i < m * n; i++) {
	fscanf(f, "%lf", matrix + i);
    }
    
    fclose(f);
}

/* Write a matrix to a file */
void matrix_write_file(int m, int n, double *matrix, char *fname) {
    FILE *f = fopen(fname, "w");
    int i, j, idx;

    if (f == NULL) {
	printf("[matrix_write_file] Error writing matrix to %s\n", fname);
	return;
    }

    idx = 0;
    for (i = 0; i < m; i++) {
	for (j = 0; j < n; j++, idx++) {
	    fprintf(f, "%0.16e ", matrix[idx]);
	}
	fprintf(f, "\n");
    }

    fclose(f);
}

// #define TEST
#ifdef TEST
int main() {
    double A[16] = { -1, 0, 1, 8,
                     1, 0, 1, 4,
                     -1, 0, 1, 2,
                     1, 1, 1, 1};
    double R[16];
    
    matrix_invert(4, A, (double *)R);

    print_matrix(4, 4, R);

    return 0;
}
#endif

/* Compute singular value decomposition of an m x n matrix A */
void dgesvd_driver(int m, int n, double *A, double *U, double *S, double *VT) {
    double *AT, *UT, *V;
    
    char jobu = 'a';
    char jobvt = 'a';

    int lda = m;
    int ldu = m;
    int ldvt = n;

    int lwork = 10 * MAX(3 * MIN(m, n) + MAX(m, n), 5 * MIN(m, n));
    double *work;

    int info;

    /* Transpose A */
    AT = (double *)malloc(sizeof(double) * m * n);    
    matrix_transpose(m, n, A, AT);

    /* Create temporary matrices for output of dgesvd */
    UT = (double *)malloc(sizeof(double) * m * m);
    V = (double *)malloc(sizeof(double) * n * n);

    work = malloc(sizeof(double) * lwork);

    dgesvd_(&jobu, &jobvt, &m, &n, AT, &lda, S, UT, &ldu, V, &ldvt, work, &lwork, &info);
    
    if (info != 0) {
		//printf("[dgesvd_driver] An error occurred\n");
    }

    matrix_transpose(m, m, UT, U);
    matrix_transpose(n, n, V, VT);

    free(AT);
    free(UT); 
    free(V);
    free(work);
}

/* Compute Cholesky decomposition of an nxn matrix */
void dpotrf_driver(int n, double *A, double *U) {
    double *AT;
    char uplo = 'U';
    int lda = n;
    int info;

    /* Transpose A */
    AT = (double *)malloc(sizeof(double) * n * n);
    matrix_transpose(n, n, A, AT);
    
    /* Call lapack routine */
    dpotrf_(&uplo, &n, AT, &lda, &info);

    /* Transpose AT */
    matrix_transpose(n, n, AT, U);

    free(AT);
}

/* Compute a QR factorization of an m by n matrix A */
void dgeqrf_driver(int m, int n, double *A, double *Q, double *R)
{
    double *AT;
    int lda = m;
    double *tau;
    int tau_dim = MIN(m, n);
    double *work;
    int block_size = 64; /* Just a guess... */
    int lwork = n * block_size;
    int info;
    double *H;
    double *v, *vvT;
    double *Qtmp;

    int i, j;

    /* Transpose A */
    AT = (double *) malloc(sizeof(double) * m * n);
    matrix_transpose(m, n, A, AT);

    /* Call the LAPACK routine */
    tau = (double *) malloc(sizeof(double) * tau_dim);
    work = (double *) malloc(sizeof(double) * lwork);
    dgeqrf_(&m, &n, AT, &lda, tau, work, &lwork, &info);

    if (info < 0) {
	printf("[dgeqrf_driver] An error occurred.\n");

	free(AT);
	free(work);
	free(tau);

	return;
    }

    /* Extract the R matrix */
    for (i = 0; i < m; i++) {
	for (j = 0; j < n; j++) {
	    if (j < i)
		R[i * n + j] = 0.0;
	    else
		R[i * n + j] = AT[j * m + i];
	}
    }


    /* Now extract the Q matrix */
    H = (double *) malloc(sizeof(double) * m * m);
    v = (double *) malloc(sizeof(double) * m);
    vvT = (double *) malloc(sizeof(double) * m * m);
    Qtmp = (double *) malloc(sizeof(double) * m * m);

    for (i = 0; i < tau_dim; i++) {
	matrix_ident(m, H);
	
	for (j = 0; j < m; j++) {
	    if (j < i)
		v[j] = 0.0;
	    else if (j == i)
		v[j] = 1.0;
	    else
		v[j] = AT[i * m + j];
	}

	matrix_transpose_product2(m, 1, m, 1, v, v, vvT);
	matrix_scale(m, m, vvT, tau[i], vvT);	
	matrix_diff(m, m, m, m, H, vvT, H);

	if (i == 0) {
	    memcpy(Q, H, sizeof(double) * m * m);
	} else {
	    matrix_product(m, m, m, m, Q, H, Qtmp);
	    memcpy(Q, Qtmp, sizeof(double) * m * m);
	}
    }

    free(H);
    free(v);
    free(vvT);
    free(Qtmp);

    free(tau);
    free(work);
    free(AT);
}

/* Compute an RQ factorization of an m by n matrix A */
void dgerqf_driver(int m, int n, double *A, double *R, double *Q)
{
    double *AT;
    int lda = m;
    double *tau;
    int tau_dim = MIN(m, n);
    double *work;
    int block_size = 64; /* Just a guess... */
    int lwork = n * block_size;
    int info;
    double *H;
    double *v, *vvT;
    double *Qtmp;

    int i, j;

    /* Transpose A */
    AT = (double *) malloc(sizeof(double) * m * n);
    matrix_transpose(m, n, A, AT);

    /* Call the LAPACK routine */
    tau = (double *) malloc(sizeof(double) * tau_dim);
    work = (double *) malloc(sizeof(double) * lwork);
    dgerqf_(&m, &n, AT, &lda, tau, work, &lwork, &info);

    if (info < 0) {
	printf("[dgeqrf_driver] An error occurred.\n");

	free(AT);
	free(work);
	free(tau);

	return;
    }

    /* Extract the R matrix */
    for (i = 0; i < m; i++) {
	for (j = 0; j < n; j++) {
	    if (j < i)
		R[i * n + j] = 0.0;
	    else
		R[i * n + j] = AT[(n - m + j) * m + i];
	}
    }


    /* Now extract the Q matrix */
    H = (double *) malloc(sizeof(double) * n * n);
    v = (double *) malloc(sizeof(double) * n);
    vvT = (double *) malloc(sizeof(double) * n * n);
    Qtmp = (double *) malloc(sizeof(double) * n * n);

    for (i = 0; i < tau_dim; i++) {
	matrix_ident(m, H);
	
	for (j = 0; j < n; j++) {
	    if (j > n - tau_dim + i)
		v[j] = 0.0;
	    else if (j == n - tau_dim + i)
		v[j] = 1.0;
	    else
		v[j] = AT[j * m + (m-tau_dim+i)];
	}

	matrix_transpose_product2(n, 1, n, 1, v, v, vvT);
	matrix_scale(n, n, vvT, tau[i], vvT);
	matrix_diff(n, n, n, n, H, vvT, H);

	if (i == 0) {
	    memcpy(Q, H, sizeof(double) * n * n);
	} else {
	    matrix_product(n, n, n, n, Q, H, Qtmp);
	    memcpy(Q, Qtmp, sizeof(double) * n * n);
	}
    }

    matrix_product(m, n, n, n, R, Q, H);

    free(H);
    free(v);
    free(vvT);
    free(Qtmp);

    free(tau);
    free(work);
    free(AT);
}

/* Convert a rotation matrix to axis and angle representation */
void matrix_to_axis_angle(double *R, double *axis, double *angle) {
    double d1 = R[7] - R[5];
    double d2 = R[2] - R[6];
    double d3 = R[3] - R[1];

    double norm = sqrt(d1 * d1 + d2 * d2 + d3 * d3);
    double x = (R[7] - R[5]) / norm;
    double y = (R[2] - R[6]) / norm;
    double z = (R[3] - R[1]) / norm;

    *angle = acos((R[0] + R[4] + R[8] - 1.0) * 0.5);

    axis[0] = x;
    axis[1] = y;
    axis[2] = z;
}

void axis_angle_to_matrix(double *axis, double angle, double *R) {
    double ident[9];
    double n[9] = { 0.0, -axis[2], axis[1],
		    axis[2], 0.0, -axis[0],
		    -axis[1], axis[0], 0.0 };

    double nsq[9], sn[9], cnsq[9], tmp[9];

    double c, s;
    
    c = cos(angle);
    s = sin(angle);

    matrix_ident(3, ident);
    matrix_product(3, 3, 3, 3, n, n, nsq);
    matrix_scale(3, 3, n, s, sn);
    matrix_scale(3, 3, nsq, (1 - c), cnsq);

    matrix_sum(3, 3, 3, 3, ident, sn, tmp);
    matrix_sum(3, 3, 3, 3, tmp, cnsq, R);
}

void axis_angle_to_matrix4(double *axis, double angle, double *R) {
    double ident[9];
    double n[9] = { 0.0, -axis[2], axis[1],
		    axis[2], 0.0, -axis[0],
		    -axis[1], axis[0], 0.0 };

    double nsq[9], sn[9], cnsq[9], tmp[9];
    double R3[9];

    double c, s;
    
    c = cos(angle);
    s = sin(angle);

    matrix_ident(3, ident);
    matrix_product(3, 3, 3, 3, n, n, nsq);
    matrix_scale(3, 3, n, s, sn);
    matrix_scale(3, 3, nsq, (1 - c), cnsq);

    matrix_sum(3, 3, 3, 3, ident, sn, tmp);
    matrix_sum(3, 3, 3, 3, tmp, cnsq, R3);

    matrix_ident(4, R);
    memcpy(R, R3, 3 * sizeof(double));
    memcpy(R + 4, R3 + 3, 3 * sizeof(double));
    memcpy(R + 8, R3 + 6, 3 * sizeof(double));
}

/* Decompose a square matrix into an orthogonal matrix and a symmetric
 * positive semidefinite matrix */
void matrix_polar_decomposition(int n, double *A, double *Q, double *S) 
{
    double *U, *diag, *VT;
    double *diag_full, *tmp;
    int i;

    U = (double *) malloc(sizeof(double) * n * n);
    diag = (double *) malloc(sizeof(double) * n);
    VT = (double *) malloc(sizeof(double) * n * n);

    /* Compute SVD */
    dgesvd_driver(n, n, A, U, diag, VT);
 
    /* Compute Q */
    matrix_product(n, n, n, n, U, VT, Q);
    
    /* Compute S */
    diag_full = (double *) malloc(sizeof(double) * n * n);

    for (i = 0; i < n * n; i++) {
	diag_full[i] = 0.0;
    }
    
    for (i = 0; i < n; i++) {
	diag_full[i * n + i] = diag[i];
    }

    tmp = (double *) malloc(sizeof(double) * n * n);
    matrix_transpose_product(n, n, n, n, VT, diag_full, tmp);
    matrix_product(n, n, n, n, tmp, VT, S);

    free(U);
    free(diag);
    free(VT);
    free(diag_full);
    free(tmp);
}

/* Find the unit vector that minimizes ||Ax|| */
void matrix_minimum_unit_norm_solution(int m, int n, double *A, double *x)
{
    /* Do an SVD */
    double *U, *S, *VT;
    int num_svs = MIN(m, n);
    int i, min_idx = -1;
    double min_sv = DBL_MAX;

    U = (double *) malloc(sizeof(double) * m * m);
    VT = (double *) malloc(sizeof(double) * n * n);
    S = (double *) malloc(sizeof(double) * sizeof(num_svs));

    dgesvd_driver(m, n, A, U, S, VT);

    /* Return the column of V associated with the smallest singular
     * value */

    for (i = 0; i < num_svs; i++) {
	if (S[i] < min_sv) {
	    min_sv = S[i];
	    min_idx = i;
	}
    }

    for (i = 0; i < n; i++) {
	x[i] = VT[min_idx * n + i];
    }

    free(U);
    free(VT);
    free(S);
}

/* Convert a matrix to a normalize quaternion */
void matrix_to_quaternion(double *R, double *q) {
    double n4; // the norm of quaternion multiplied by 4 
    double tr = R[0] + R[4] + R[8]; // trace of martix
    double factor;

    if (tr > 0.0) {
        q[1] = R[5] - R[7];
	q[2] = R[6] - R[2];
	q[3] = R[1] - R[3];
	q[0] = tr + 1.0;
        n4 = q[0];
    } else if ((R[0] > R[4]) && (R[0] > R[8])) {
        q[1] = 1.0 + R[0] - R[4] - R[8];
	q[2] = R[3] + R[1];
        q[3] = R[6] + R[2];
	q[0] = R[5] - R[7];
        n4 = q[1];
    } else if (R[4] > R[8]) {
        q[1] = R[3] + R[1];
	q[2] = 1.0 + R[4] - R[0] - R[8];
        q[3] = R[7] + R[5];
	q[0] = R[6] - R[2]; 
        n4 = q[2];
    } else {
        q[1] = R[6] + R[2];
	q[2] = R[7] + R[5];
	q[3] = 1.0 + R[8] - R[0] - R[4];
	q[0] = R[1] - R[3];
        n4 = q[3];
    }
    
    factor = 0.5 / sqrt(n4);
    q[0] *= factor;
    q[1] *= factor;
    q[2] *= factor;
    q[3] *= factor;
}

/* Convert a normalized quaternion to a matrix */
void quaternion_to_matrix(double *q, double *R) {
    double sqw = q[0] * q[0];
    double sqx = q[1] * q[1]; 
    double sqy = q[2] * q[2];
    double sqz = q[3] * q[3];

    double tmp1, tmp2;

    R[0] =  sqx - sqy - sqz + sqw; // since sqw + sqx + sqy + sqz = 1
    R[4] = -sqx + sqy - sqz + sqw;    
    R[8] = -sqx - sqy + sqz + sqw;

    tmp1 = q[1] * q[2];
    tmp2 = q[3] * q[0];

    R[1] = 2.0 * (tmp1 + tmp2);    
    R[3] = 2.0 * (tmp1 - tmp2);        

    tmp1 = q[1] * q[3];    
    tmp2 = q[2] * q[0];    

    R[2] = 2.0 * (tmp1 - tmp2);    
    R[6] = 2.0 * (tmp1 + tmp2);    

    tmp1 = q[2] * q[3];    
    tmp2 = q[1] * q[0];    

    R[5] = 2.0 * (tmp1 + tmp2);   
    R[7] = 2.0 * (tmp1 - tmp2);
}
