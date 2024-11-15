# HPCA Assignment
Optimize checkered matrix multiplication using hardware counters.


* Contains setup for multi-threaded program and GPU program.
* Makefile: Contains commands necessary to compile, generate inputs, and run the program.
* data folder: Contains program that generates input, and will contain input once generated.
* header folder: Files containing the function that performs the operation. Modify the files in this folder.
* report folder: Add your report here as a PDF.
* main.cu: Program that takes inputs and executes the functions. DO NOT MODIFY THIS.

You can do the following to setup:
### Compiling and generating input
Use the following command to compile the programs and generate required input:
```
make
```

To compile the code for use on native GPU use the following command:
```
make server
```
For use with GPGPU-Sim, additional flags are required during compilation, which can be done with the following command:
```
make sim
```

You can use make to run the executable with the following command for native execution:
```
make run_server INPUT_SIZE=1024
```
When running on GPGPU-Sim, use the following command instead:
```
make run_sim INPUT_SIZE=1024
```
