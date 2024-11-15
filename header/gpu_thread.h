#include <cuda_runtime.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Feel free to modify any functions already defined in this file

// Create extra functions here (if, any)

// CUDA kernel
__global__ void computeKernel(int N, int *matA, int *matB, int *output)
{
    for (int rowA = 0; rowA < N; rowA++)
    {
        for (int colSetB = 0; colSetB < N; colSetB += 2)
        {
            int sum = 0;

            if (rowA & 1)
            {
                // Handle odd rows in matA

                // Iterate over even positions in rowA and odd positions in column colSetB in matB
                for (int indexA = rowA * N, indexB = colSetB + N;
                     indexA < (rowA + 1) * N; indexA += 2, indexB += (N << 1))
                {
                    sum += matA[indexA] * matB[indexB];
                }

                // Iterate over odd positions in rowA and even positions in column colSetB+1 in matB
                for (int indexA = rowA * N + 1, indexB = colSetB + 1;
                     indexA < (rowA + 1) * N; indexA += 2, indexB += (N << 1))
                {
                    sum += matA[indexA] * matB[indexB];
                }
            }
            else
            {
                // Handle even rows in matA

                // Iterate over even positions in rowA and odd positions in column colSetB+1 in matB
                for (int indexA = rowA * N, indexB = colSetB + 1 + N;
                     indexA < (rowA + 1) * N; indexA += 2, indexB += (N << 1))
                {
                    sum += matA[indexA] * matB[indexB];
                }

                // Iterate over odd positions in rowA and even positions in column colSetB in matB
                for (int indexA = rowA * N + 1, indexB = colSetB;
                     indexA < (rowA + 1) * N; indexA += 2, indexB += (N << 1))
                {
                    sum += matA[indexA] * matB[indexB];
                }
            }

            // Compute output indices
            int rowC = rowA >> 1;                              // Row in output
            int colC = (colSetB >> 1) + (N >> 1) * (rowA & 1); // Column in output
            int indexC = rowC * N + colC;                      // Flattened output index

            output[indexC] = sum;
        }
    }
}

// Host function to launch the CUDA kernel
void gpuThread(int N, int *matA, int *matB, int *output)
{
    assert(N >= 4 && N == (N & ~(N - 1))); // Ensure N is a power of 2 and >= 4

    // Allocate device memory
    int *d_matA, *d_matB, *d_output;
    size_t size = N * N * sizeof(int);             // Size for matA and matB
    size_t outputSize = N * (N / 2) * sizeof(int); // Size for output matrix

    cudaMalloc(&d_matA, size);
    cudaMalloc(&d_matB, size);
    cudaMalloc(&d_output, outputSize);

    // Copy data to device
    cudaMemcpy(d_matA, matA, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_matB, matB, size, cudaMemcpyHostToDevice);
    cudaMemset(d_output, 0, outputSize);

    // Define block and grid dimensions
    int threadsPerBlock = 1;                                     // Number of threads per block
    int numBlocks = (N + threadsPerBlock - 1) / threadsPerBlock; // Grid size

    // Launch the kernel
    computeKernel<<<numBlocks, threadsPerBlock>>>(N, d_matA, d_matB, d_output);

    // Wait for GPU to finish
    cudaDeviceSynchronize();

    // Copy results back to host
    cudaMemcpy(output, d_output, outputSize, cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_matA);
    cudaFree(d_matB);
    cudaFree(d_output);
}
