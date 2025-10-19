#include <iostream>
#include <omp.h>

int main() {
    int N = 1000000;
    int *A = new int[N];
    int *B = new int[N];
    int *C = new int[N];

    // Initialize vectors
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = 2 * i;
    }

    // Parallel vector addition using OpenMP
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        C[i] = A[i] + B[i];
    }

    // Print first 10 results
    for (int i = 0; i < 10; i++) {
        std::cout << "C[" << i << "] = " << C[i]
                  << " (Expected: " << A[i] + B[i] << ")\n";
    }

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}

g++ vector_add.cpp -fopenmp -o vector_add
./vector_add
