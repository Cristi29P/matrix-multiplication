/*
 * Tema 2 ASC
 * 2022 Spring
 */
#include "utils.h"

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
 * Add your optimized implementation here
 */
double* my_solver(int N, double *A, double* B) {
	/* Using registers is faster because the data is stored
	 * as closely to the processor as possible. 
	 * They are also working at processor speed. 
	 */
	register double *M1, *M2, *M2_start, *Bt_start, *B_start, *M2_start_cpy, *B_start_cpy;
	register double *M1_start, *B1_start, *A1_start, *B1_start_cpy;
	register double *M2_start2, *M1_start2, *M1_start2_cpy, *A2_start;
	register int i, j, k;
	register double sum;
	register int constant;

	M1 = calloc(N * N, sizeof(double));
	DIE(!M1, "Calloc M1 failed.");

	M2 = calloc(N * N, sizeof(double));
	DIE(!M2, "Calloc M2 failed.");

	/* C = B * A * A_t + B_t * B */

	/* Calculate B * A and store it in M1. */
	for (i = 0; i != N; ++i) {
		constant = i * N; /* Define constant to save some multiply operations */
		M1_start = M1 + constant; /* Line i from M1 */
		B1_start = B + constant; /* Line i from B */

		for (j = 0; j != N; ++j) {
			A1_start = A + j; /* Column j from A */
			B1_start_cpy = B1_start; /* Backup line start to make sure we don't make any extra iterations */
			sum = 0.0; /* Partial sum */

			for (k = 0; k <= j; ++k) {
				sum += *B1_start_cpy * *A1_start; /* B[i * N + k] * A[k * N + j] */
				A1_start += N; /* Increment by line size to go to next column element */
				++B1_start_cpy; /* Increment by 1 to go to next line element */
			}

			*M1_start = sum; /* Store the sum at M1[i * N + j]*/
			++M1_start; /* Go to next line element */
		}
	}
		
	/* Calculate M1 * A_t and store it in M2 */
	for (i = 0; i != N; ++i) {
		constant = i * N; /* Define constant to save some multiply operations */
		M2_start2 = M2 + constant; /* Line i from M1 */
		M1_start2 = M1 + constant; /* Line i from M2 */

		for (j = 0; j != N; ++j) {
			M1_start2_cpy = M1_start2 + j; /* Line i, column j of M2 */
			A2_start = A + j * N + j; /* Line j, column j of A */
			sum = 0.0; /* Partial sum */

			for (k = j; k != N; ++k) {
				sum += *M1_start2_cpy * *A2_start; /* M1[i * N + k] * A[j * N + k] */
				++A2_start; /* Go to next line element */
				++M1_start2_cpy; /* Go to next line element */
			}

			*M2_start2 = sum; /* Store the sum at M2[i * N + j] */
			++M2_start2; /* Go to next line element */
		}
	}

	/* Add B_t * B to M2
	 * Switch up the for loop arrangement to better utilize the data found in cache.
	 * The i-k-j order assures us we have at most sequential cache operations. 
	 */
	for (i = 0; i != N; ++i) {
		M2_start = M2 + i * N; /* Line i of M2 */
		Bt_start = B + i; /* Column i of B */
		B_start = B; /* First line of B */

		for (k = 0; k != N; ++k) {
			M2_start_cpy = M2_start; /* Backup the pointers to don't make extra iterations */
			B_start_cpy = B_start;

			for (j = 0; j != N; ++j) {
				*M2_start_cpy += *Bt_start * *B_start_cpy; /* B[k * N + i] * B[k * N + j] */
				++M2_start_cpy; /* Go to next line element */
				++B_start_cpy; /* Go to next line element (of the transposed matrix). */
			}

			Bt_start += N; /* Go to next column element */
			B_start += N; /* Go to next line */
		}
	}
		
	free(M1);
	return M2;
}
