Code:
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>
#include <iomanip>
#include <omp.h>
#include <string>

// ---------- Utility Functions ----------
bool is_sorted_nondec(const std::vector<int>& a) {
    for (size_t i = 1; i < a.size(); ++i)
        if (a[i] < a[i - 1]) return false;
    return true;
}

// ---------- Parallel Merge Sort ----------
void merge(std::vector<int>& a, int left, int mid, int right, std::vector<int>& temp) {
    int i = left, j = mid, k = left;
    while (i < mid && j < right) {
        if (a[i] <= a[j]) temp[k++] = a[i++];
        else temp[k++] = a[j++];
    }
    while (i < mid) temp[k++] = a[i++];
    while (j < right) temp[k++] = a[j++];
    for (i = left; i < right; ++i) a[i] = temp[i];
}

void parallel_mergesort_rec(std::vector<int>& a, int left, int right, std::vector<int>& temp, int grain) {
    if (right - left <= grain) {
        std::sort(a.begin() + left, a.begin() + right);
        return;
    }
    int mid = left + (right - left) / 2;
    #pragma omp task shared(a,temp) if(right-left>grain)
    parallel_mergesort_rec(a, left, mid, temp, grain);
    #pragma omp task shared(a,temp) if(right-left>grain)
    parallel_mergesort_rec(a, mid, right, temp, grain);
    #pragma omp taskwait
    merge(a, left, mid, right, temp);
}

void parallel_mergesort(std::vector<int>& a, int threads) {
    std::vector<int> temp(a.size());
    #pragma omp parallel num_threads(threads)
    {
        #pragma omp single
        parallel_mergesort_rec(a, 0, a.size(), temp, 1000); // grain size=1000
    }
}

// ---------- Parallel Bitonic Sort ----------
void compare_swap(int &a, int &b, bool dir) {
    if ((a > b) == dir) std::swap(a, b);
}

void bitonic_merge(std::vector<int>& a, int low, int cnt, bool dir) {
    if (cnt > 1) {
        int k = cnt/2;
        #pragma omp parallel for
        for (int i = low; i < low + k; ++i) compare_swap(a[i], a[i+k], dir);
        bitonic_merge(a, low, k, dir);
        bitonic_merge(a, low + k, k, dir);
    }
}

void bitonic_sort_rec(std::vector<int>& a, int low, int cnt, bool dir) {
    if (cnt > 1) {
        int k = cnt/2;
        bitonic_sort_rec(a, low, k, true);
        bitonic_sort_rec(a, low + k, k, false);
        bitonic_merge(a, low, cnt, dir);
    }
}

void bitonic_sort_parallel(std::vector<int>& a, int threads) {
    // Pad to next power of 2
    size_t N = a.size();
    size_t pow2 = 1;
    while (pow2 < N) pow2 <<= 1;
    a.resize(pow2, std::numeric_limits<int>::max()); // sentinel
    
    bitonic_sort_rec(a, 0, pow2, true);
    
    // Remove padding
    a.resize(N);
}

// ---------- Main ----------
int main(int argc, char** argv) {
    size_t N = 1000000;
    int threads = 4;
    std::string algo = "mergesort";
    bool check = true;
    unsigned seed = 123;

    auto next = [&](int &i){ return argv[++i]; };
    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        if(s=="-n" || s=="--size"){ N = std::stoull(next(i)); ++i; }
        else if(s=="-t" || s=="--threads"){ threads = std::stoi(next(i)); ++i; }
        else if(s=="-a" || s=="--algo"){ algo = next(i); ++i; }
        else if(s=="--no-check"){ check = false; }
        else if(s=="--seed"){ seed = std::stoul(next(i)); ++i; }
        else if(s=="-h" || s=="--help"){
            std::cerr << "Usage: " << argv[0] 
                      << " [-n N] [-t THREADS] [-a mergesort|bitonic] [--no-check] [--seed S]\n";
            return 0;
        }
    }

    std::vector<int> a(N);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(),
                                            std::numeric_limits<int>::max());
    for(size_t i=0;i<N;++i) a[i] = dist(rng);

    std::vector<int> a_copy = a;

    // Serial sort
    double t0 = omp_get_wtime();
    std::vector<int> serial = a_copy;
    std::sort(serial.begin(), serial.end());
    double t1 = omp_get_wtime();

    // Parallel sort
    std::vector<int> para = a_copy;
    double p0 = omp_get_wtime();
    if(algo == "mergesort") parallel_mergesort(para, threads);
    else if(algo == "bitonic") bitonic_sort_parallel(para, threads);
    else { std::cerr << "Unknown algo: " << algo << "\n"; return 1; }
    double p1 = omp_get_wtime();

    if(check){
        if(!is_sorted_nondec(para)){ std::cerr << "ERROR: Parallel result is NOT sorted!\n"; return 2; }
        if(para.size() != serial.size()){ std::cerr << "ERROR: Size mismatch!\n"; return 3; }
        if(para != serial){ std::cerr << "WARNING: Parallel result differs from serial.\n"; }
    }

    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(6);
    std::cout << "Input size: " << N << "\n";
    std::cout << "Threads: " << threads << "\n";
    std::cout << "Algorithm: " << algo << "\n";
    std::cout << "Serial sort time (s): " << (t1 - t0) << "\n";
    std::cout << "Parallel sort time (s): " << (p1 - p0) << "\n";
    std::cout << "Speedup (serial/parallel): " << (t1 - t0) / (p1 - p0) << "\n";

    return 0;
}


