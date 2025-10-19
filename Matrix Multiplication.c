#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1024 // Matrix size (N x N)

void matrix_multiply(double** A, double** B, double** C, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

double** alloc_matrix(int n) {
    double** mat = malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        mat[i] = malloc(n * sizeof(double));
    }
    return mat;
}

void free_matrix(double** mat, int n) {
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

int main() {
    double** A = alloc_matrix(N);
    double** B = alloc_matrix(N);
    double** C = alloc_matrix(N);

    // Initialize matrices
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }
    }

    double start = omp_get_wtime();
    matrix_multiply(A, B, C, N);
    double end = omp_get_wtime();

    printf("C[0][0] = %f\n", C[0][0]);
    printf("Time taken: %f seconds\n", end - start);

    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);

    return 0;
}

gcc matrix_multiply.c -fopenmp -o matrix_multiply
./matrix_multiply
