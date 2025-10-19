nano parallel_matrix.cpp

#include <iostream>
#include <omp.h>
#include <vector>
#include <iomanip> // for setw and setprecision

using namespace std;

int main() {
    int n = 500; // Matrix size
    vector<vector<double>> A(n, vector<double>(n));
    vector<vector<double>> B(n, vector<double>(n));
    vector<vector<double>> C(n, vector<double>(n));

    // Initialize matrices
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }

    // Sequential multiplication
    double start_seq = omp_get_wtime();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    double end_seq = omp_get_wtime();
    double T_seq = end_seq - start_seq;

    cout << fixed << setprecision(4); // set decimal precision
    cout << "Sequential Execution Time: " << T_seq << " seconds\n";

    // Table header
    cout << "\nThreads  Parallel Time  Speedup  Efficiency(%)\n";
    cout << "-----------------------------------------------\n";

    // Parallel multiplication with different thread counts
    for (int threads = 2; threads <= 8; threads *= 2) {
        vector<vector<double>> Cpar(n, vector<double>(n));
        double start = omp_get_wtime();

        #pragma omp parallel for num_threads(threads) collapse(2)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                double sum = 0;
                for (int k = 0; k < n; k++)
                    sum += A[i][k] * B[k][j];
                Cpar[i][j] = sum;
            }

        double end = omp_get_wtime();
        double T_par = end - start;
        double speedup = T_seq / T_par;
        double efficiency = (speedup / threads) * 100;

        // Nicely formatted output
        cout << setw(8) << threads
             << setw(15) << T_par
             << setw(10) << speedup
             << setw(12) << efficiency
             << endl;
    }

    return 0;
}

g++ -fopenmp parallel_matrix.cpp -o parallel_matrix
./parallel_matrix
