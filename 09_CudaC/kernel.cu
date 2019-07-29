#include <cuda.h>
#include "cuda_runtime.h"
#include "cuda_runtime_api.h"
#include "device_launch_parameters.h"
#include "device_functions.h"

#include <iostream>
#include <stdio.h>
#include <ctime>

void random_longs(long* a, int size) //randomizing elements in both vectors
{
	srand(time(0));
	int i;
	for (i = 0; i < size; ++i)
	{
		a[i] = rand() % 100;
	}
}

__global__ void add(long* a, long* b, long* c, long N) { //core from ScalarMultiplication_example1
	long baseIdx = threadIdx.x;
	long idx = baseIdx;
	while (idx < N)
	{
		c[idx] = a[idx] * b[idx];
		idx += blockDim.x;
	}
	__syncthreads();
	long step = N / 2;
	while (step != 0) {
		idx = baseIdx;
		while (idx < step) {
			c[idx] += c[idx + step];
			idx += blockDim.x;
		}
		step /= 2;
		__syncthreads();
	}
}

__global__ void add_1024(long* a, long* b, long* c, long N) { //more simple and probably faster core but works only with 1024 or less elements in vector in this example
	c[threadIdx.x] = a[threadIdx.x] * b[threadIdx.x];
	__syncthreads();
	long step = N / 2;
	while (step != 0) {
		if (threadIdx.x < step)
		{
			c[threadIdx.x] += c[threadIdx.x + step];
		}
		step /= 2;
		__syncthreads();
	}
}

int main(void) {
	long N;
	clock_t start_t_gpu, start_t_cpu, end_t_gpu, end_t_cpu;
	long res_CPU;
	long* a, * b, * c, * d; // host copies of a, b, c
	long* d_a, * d_b, * d_c; // device copies of a, b, c
	int size;
	for (int i = 0; i < 3; i++)
	{
		if (i == 0) N = 512;
		if (i == 1) N = 1024;
		if (i == 2) N = 131072;
		size = N * sizeof(long);
		std::cout << "Vector size: " << N << std::endl;
		// Alloc space for device copies of a, b, c
		cudaMalloc((void**)& d_a, size);
		cudaMalloc((void**)& d_b, size);
		cudaMalloc((void**)& d_c, size);
		// Alloc space for host copies of a, b, c and setup input values
		a = (long*)malloc(size); random_longs(a, N);
		b = (long*)malloc(size); random_longs(b, N);
		c = (long*)malloc(sizeof(long));
		d = (long*)malloc(size);
		// Copy inputs to device
		cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
		cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);
		// Launch add() kernel on GPU with N blocks

		if (N <= 1024)
		{
			start_t_gpu = clock();
			for (int i = 0; i < 10000; i++)
			{
				add_1024 << <1, N >> > (d_a, d_b, d_c, N);
			}
			end_t_gpu = clock();
			cudaMemcpy(c, d_c, sizeof(long), cudaMemcpyDeviceToHost);
			// Copy result back to host
			std::cout << "ScalarMultiplication on GPU (simple core): " << std::endl << "result: " << c[0] << std::endl;
			std::cout << "time: " << ((double)end_t_gpu - start_t_gpu)/ CLOCKS_PER_SEC << " seconds" << std::endl;
		}
		else {
			start_t_gpu = clock();
			for (int i = 0; i < 10000; i++)
			{
				add << <1, 1024 >> > (d_a, d_b, d_c, N);
			}
			cudaMemcpy(c, d_c, sizeof(long), cudaMemcpyDeviceToHost);
			end_t_gpu = clock();
			// Copy result back to host
			std::cout << "ScalarMultiplication on GPU (core from ScalarMultiplication_example1): " << std::endl << "result: " << c[0] << std::endl;
			std::cout << "time: " << ((double)end_t_gpu - start_t_gpu)/ CLOCKS_PER_SEC << " seconds" << std::endl;
		}

		start_t_cpu = clock();
		for (int i = 0; i < 10000; i++) //cycle just to see average time
		{
			res_CPU = 0;
			for (long i = 0; i < N; ++i)
			{
				res_CPU += a[i] * b[i];
			}
		}
		end_t_cpu = clock();
		std::cout << "ScalarMultiplication on CPU: " << std::endl << "result: " << res_CPU << std::endl;
		std::cout << "time: " << ((double)end_t_cpu - start_t_cpu)/ CLOCKS_PER_SEC << " seconds" << std::endl << std::endl;
		// Cleanup
		free(a); free(b); free(c);
		cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);
	}
	return 0;
}