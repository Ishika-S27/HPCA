#include <stdio.h>
#include <omp.h>
#define SIZE 1000
int main() {
int i;
int a[SIZE], b[SIZE], c[SIZE];
// Initialize arrays
for (i = 0; i < SIZE; i++) {
a[i] = i;
b[i] = i * 2;
}
printf("Parallel for loop using different scheduling
strategies:\n\n");
// STATIC scheduling
printf("STATIC scheduling:\n");
#pragma omp parallel for schedule(static, 100) shared(a, b, c)
private(i)
for (i = 0; i < SIZE; i++) {
c[i] = a[i] + b[i];
printf("Thread %d processes index %d\n",
omp_get_thread_num(), i);
}
// DYNAMIC scheduling
printf("\nDYNAMIC scheduling:\n");
#pragma omp parallel for schedule(dynamic, 100) shared(a, b, c)
private(i)
for (i = 0; i < SIZE; i++) {
c[i] = a[i] + b[i];
printf("Thread %d processes index %d\n",
omp_get_thread_num(), i);
}
// GUIDED scheduling
printf("\nGUIDED scheduling:\n");
#pragma omp parallel for schedule(guided, 100) shared(a, b, c)
private(i)
for (i = 0; i < SIZE; i++) {
c[i] = a[i] + b[i];
printf("Thread %d processes index %d\n",
omp_get_thread_num(), i);
}
return 0;
}
