/*
 * Tema 2 ASC
 * 2022 Spring
 */
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "cblas.h"

#define DIE(assertion, call_description)					\
	do {									\
		if (assertion) {						\
			fprintf(stderr, "(%s, %s, %d): ",			\
					__FILE__, __FUNCTION__, __LINE__);	\
			perror(call_description);			\
			exit(EXIT_FAILURE);					\
		}								\
	} while (0)

/* 
 * Add your BLAS implementation here
 */
double* my_solver(int N, double *A, double *B) {
	double *M1, *M2;

	M1 = calloc(N * N, sizeof(double));
	DIE(!M1, "Calloc M1 failed.");

	M2 = calloc(N * N, sizeof(double));
	DIE(!M2, "Calloc M1 failed.");

	/* C = B * A * A_t + B_t * B */

	/* Copy A to M1 */
	cblas_dcopy(N * N, A, 1, M1, 1);
	
	/* M1 = A * A_t
	 * A is upper triangular, so we use dtrmm to take advantage of that property.
	 */
	cblas_dtrmm(CblasRowMajor, 
				CblasRight,
				CblasUpper,
				CblasTrans,
				CblasNonUnit,
				N, N, 1.0, A, 
				N, M1, N);

	/* M2 = B * (A * A_t) = B * M1 
	 * The M1 matrix is symmetric (A * A_t, product of an upper triangular matrix
	 * and a lower triangular one), so we use dsymm to take advantage of that 
	 */
	cblas_dsymm(CblasRowMajor,
				CblasRight,
				CblasUpper,
				N, N, 1.0, M1, N, 
				B, N, 0.0, M2, N);

	/* M2 += B_t * B 
	 * The dgemm function can simultaneously multiply two matrices and add the result 
	 * to another matrix, so there is no need for more auxiliary data space.
	 */
	cblas_dgemm(CblasRowMajor,
				CblasTrans,
				CblasNoTrans,
				N, N, N, 1.0, B,
				N, B, N, 1.0, M2, N);

	free(M1);
	return M2;
}
