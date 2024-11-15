#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>
#include <assert.h>
using namespace std;

#define TIME_NOW std::chrono::high_resolution_clock::now()
#define TIME_DIFF(gran, start, end) std::chrono::duration_cast<gran>(end - start).count()

#include "gpu_thread.h"
#include "cpu_thread.h"

// Used to cross-check answer. DO NOT MODIFY!

int main(int argc, char *argv[])
{
    // Input size of square matrices// Input size of square matrices
    int N;
    string file_name; 
    if (argc < 2) 
        file_name = "data/input_128.in"; 
    else 
        file_name = argv[1]; 
    ifstream input_file; 
    input_file.open(file_name); 
    input_file >> N;
    cout << "Input matrix of size " << N << "\n";
    
    // Input matrix A
    int *matA = new int[N * N];
    for(int i = 0; i < N; ++i)
        for(int j = 0; j < N; ++j)
            input_file >> matA[i * N + j];

    // Input matrix B
    int *matB = new int[N * N];
    for(int i = 0; i < N; ++i)
        for(int j = 0; j < N; ++j)
            input_file >> matB[i * N + j];
    
    auto begin = TIME_NOW;
    // Execute reference program
    int *output_reference = new int[N*(N>>1)];
    multiThread(N, matA, matB, output_reference);
    auto end = TIME_NOW;
    cout << "Reference execution time: " << 
        (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";    

    // Execute gpu version
    int *output_gpu = new int[N*(N>>1)];
    begin = TIME_NOW;
    gpuThread(N, matA, matB, output_gpu);
    end = TIME_NOW;
    cout << "GPU execution time: " << 
        (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";

    for(int i = 0; i < N*(N>>1); ++i)
        if(output_gpu[i] != output_reference[i]) {
            cout << "Mismatch at " << i << "\n";
            cout << "GPU output: " << output_gpu[i] << ", required output: " << output_reference[i] << "\n";
            exit(0);
        }
    input_file.close(); 
}
