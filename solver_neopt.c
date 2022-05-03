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
 * Add your unoptimized implementation here
 */
double* my_solver(int N, double *A, double* B) {
	double *M1, *M2;

	M1 = calloc(N * N, sizeof(double));
	DIE(!M1, "Calloc M1 failed.");

	M2 = calloc(N * N, sizeof(double));
	DIE(!M2, "Calloc M2 failed.");

	/* C = B * A * A_t + B_t * B, where A is upper triangular */ 
	
	/* Calculate B * A and store it in M1. The k-loop goes from 0 to j to ignore 
	 * the 0's below the main diagonal. This way, we achieve less iterations.
	 */
	for (int i = 0; i != N; ++i)
		for (int j = 0; j != N; ++j)
			for (int k = 0; k <= j; ++k)
				M1[i * N + j] += B[i * N + k] * A[k * N + j];
	
	/* Calculate M1 * A_t and store it in M2. We know that the transpose of A
	 * (upper triangular) is a lower triangular matrix. So, the k-loop goes from
	 * j to N to ignore, this time, the 0's above the main diagonal.
	 * The transpose is calculated in-place by switching the indexes. 
	 */
	for (int i = 0; i != N; ++i)
		for (int j = 0; j != N; ++j)
			for (int k = j; k != N; ++k)
				M2[i * N + j] += M1[i * N + k] * A[j * N + k];
	
	/* Add B_t * B to M2
	 * The transpose is calculated in-place by switching the indexes 
	 */
	for (int i = 0; i != N; ++i)
		for (int j = 0; j != N; ++j)
			for (int k = 0; k != N; ++k)
				M2[i * N + j] += B[k * N + i] * B[k * N + j];

	free(M1);
	return M2;
}
