#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double *A, *B, *C, *Afirsthalf, *Asecondhalf;
double AfirstAvg, AsecondAvg;
int N;

typedef struct {
    int start;
    int end;
} Range;
void insertionSort(double arr[], int n) {
    int i, j;
    double key;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}
void* sorting_avg(void* args) {
    Range* r = (Range*)args;
    int size = r->end - r->start + 1;
    double sum = 0;

    insertionSort(A + r->start, size);
    
    for(int i = r->start; i <= r->end; i++) {
        sum += A[i];
    }

    if(r->start == 0) {
        AfirstAvg = sum / size;
    } else {
        AsecondAvg = sum / size;
    }

    return NULL;
}
void* merging_avg(void* args) {
    int i = 0, j = N/2, k = 0;
    
    while (i < N/2 && j < N) {
        if (A[i] < A[j]) {
            C[k++] = A[i++];
        } else {
            C[k++] = A[j++];
        }
    }

    while (i < N/2) {
        C[k++] = A[i++];
    }

    while (j < N) {
        C[k++] = A[j++];
    }

    // Since the size of both halves are equal, taking average of averages works
    double overallAvg = (AfirstAvg + AsecondAvg) / 2;
    printf("Overall Average (Two threads): %f\n", overallAvg);
    printf("First 10 sorted elements (Two threads):\n");
    for(i = 0; i < 10; i++) {
        printf("%f ", C[i]);
    }
    printf("\n");
    return NULL;
}
int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: %s <N>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    A = malloc(N * sizeof(double));
    B = malloc(N * sizeof(double));
    C = malloc(N * sizeof(double));
    Afirsthalf = malloc(N/2 * sizeof(double));
    Asecondhalf = malloc(N/2 * sizeof(double));
    
    srand(time(NULL));
    for(int i = 0; i < N; i++) {
        A[i] = 1 + (rand() / (double)RAND_MAX) * 999;
    }

    // TODO: Complete rest of the main function following the high-level pseudocode provided.
    struct timespec ts_begin, ts_end;
    double elapsed;  

    // Copy A into B
    for(int i = 0; i < N; i++) {
        B[i] = A[i];
    }

    // Start timer for one thread case
    clock_gettime(CLOCK_MONOTONIC, &ts_begin);  

    pthread_t thB;
    Range rB = {0, N-1};
    pthread_create(&thB, NULL, sorting_avg, &rB);
    pthread_join(thB, NULL);

    // End timer for one thread case
    clock_gettime(CLOCK_MONOTONIC, &ts_end);  
    elapsed = (ts_end.tv_sec - ts_begin.tv_sec) + (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;

    printf("Sorting by ONE thread is done in %f ms\n", elapsed * 1000);
    printf("Average (One thread): %f\n", (AfirstAvg + AsecondAvg) / 2);
    printf("First 10 sorted elements (One thread):\n");
    for(int i = 0; i < 10; i++) {
        printf("%f ", B[i]);
    }
    printf("\n\n");
    // Copy A into Afirsthalf and Asecondhalf
    for(int i = 0; i < N/2; i++) {
        Afirsthalf[i] = A[i];
        Asecondhalf[i] = A[i + N/2];
    }

    // Start timer for two threads case
    clock_gettime(CLOCK_MONOTONIC, &ts_begin);  

    pthread_t thA1, thA2, thM;
    Range rA1 = {0, N/2 - 1};
    Range rA2 = {N/2, N - 1};
    pthread_create(&thA1, NULL, sorting_avg, &rA1);
    pthread_create(&thA2, NULL, sorting_avg, &rA2);

    pthread_join(thA1, NULL);
    pthread_join(thA2, NULL);

    pthread_create(&thM, NULL, merging_avg, NULL);
    pthread_join(thM, NULL);

    // End timer for two threads case
    clock_gettime(CLOCK_MONOTONIC, &ts_end);  
    elapsed = (ts_end.tv_sec - ts_begin.tv_sec) + (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;

    printf("Sorting by TWO threads is done in %f ms\n", elapsed * 1000);




    free(A);
    free(B);
    free(C);
    free(Afirsthalf);
    free(Asecondhalf);
    return 0;
}
