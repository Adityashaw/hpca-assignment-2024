#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

int getNumCores()
{
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW;
    nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if (count < 1)
    {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if (count < 1)
        {
            count = 1;
        }
    }
    return count;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}


// Structure to pass data to threads
struct ThreadData {
    int N;           // Matrix size
    int *matA;       // Pointer to matrix A
    int *matB;       // Pointer to matrix B
    int *output;     // Pointer to output matrix
    int startRow;    // Starting row for the thread
    int endRow;      // Ending row for the thread
};

// Thread function
void *computeRows(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    int N = data->N;
    int *matA = data->matA;
    int *matB = data->matB;
    int *output = data->output;

    for (int rowA = data->startRow; rowA < data->endRow; rowA++) {
        for (int colSetB = 0; colSetB < N; colSetB += 2) {
            int sum = 0;

            if (rowA & 1) {
                // Handle odd rows in matA

                // Iterate over even positions in rowA and odd positions in column colSetB in matB
                for (int indexA = rowA * N, indexB = colSetB + N;
                     indexA < (rowA + 1) * N; indexA += 2, indexB += (N << 1)) {
                    sum += matA[indexA] * matB[indexB];
                }

                // Iterate over odd positions in rowA and even positions in column colSetB+1 in matB
                for (int indexA = rowA * N + 1, indexB = colSetB + 1;
                     indexA < (rowA + 1) * N; indexA += 2, indexB += (N << 1)) {
                    sum += matA[indexA] * matB[indexB];
                }
            } else {
                // Handle even rows in matA

                // Iterate over even positions in rowA and odd positions in column colSetB+1 in matB
                for (int indexA = rowA * N, indexB = colSetB + 1 + N;
                     indexA < (rowA + 1) * N; indexA += 2, indexB += (N << 1)) {
                    sum += matA[indexA] * matB[indexB];
                }

                // Iterate over odd positions in rowA and even positions in column colSetB in matB
                for (int indexA = rowA * N + 1, indexB = colSetB;
                     indexA < (rowA + 1) * N; indexA += 2, indexB += (N << 1)) {
                    sum += matA[indexA] * matB[indexB];
                }
            }

            // Compute output indices
            int rowC = rowA >> 1;
            int colC = (colSetB >> 1) + (N >> 1) * (rowA & 1);
            int indexC = rowC * N + colC;

            output[indexC] = sum;
        }
    }

    pthread_exit(NULL);
}

// Main function to launch threads
void multiThread(int N, int *matA, int *matB, int *output) {
    assert(N >= 4 && N == (N & ~(N - 1))); // Ensure N is a power of 2 and >= 4


    int numThreads = getNumCores() - 1;
    pthread_t threads[numThreads];
    struct ThreadData threadData[numThreads];
    int rowsPerThread = N / numThreads;

    // Create threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].N = N;
        threadData[i].matA = matA;
        threadData[i].matB = matB;
        threadData[i].output = output;
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i == numThreads - 1) ? N : (i + 1) * rowsPerThread;

        pthread_create(&threads[i], NULL, computeRows, (void *)&threadData[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
}
