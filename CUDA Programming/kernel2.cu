#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

__global__ void mandelKernel(int* result, float lowerX, float lowerY, float stepX, float stepY, int iter, int resX, int resY, int pitch) {
    // To avoid error caused by the floating number, use the following pseudo code
    //
    
    int thisY = threadIdx.y + blockIdx.y * blockDim.y;
    int thisX = threadIdx.x + blockIdx.x * blockDim.x;
    // int* row = (int*)((char*)result+thisX*pitch);
    if (thisY >= resY || thisX >= resX)
        return;
    float x = lowerX + (float)thisX * stepX;
    float y = lowerY + (float)thisY * stepY;
    float x0 = x, y0 = y;
    
    int i;
    for (i = 0; i < iter; i++)
    {
        if (x * x + y * y > 4.f)
            break;
        
        float new_x = x * x - y * y;
        float new_y = 2.f * x * y;
        x = new_x + x0;
        y = new_y + y0;
    }
    result[thisY*pitch+thisX] = i;
    return;
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
		float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    int N = resX*resY;
		// �ӽ�host�O����
	  size_t in = N * sizeof(int);
    int* result;
    
    cudaHostAlloc(&result, in, cudaHostAllocPortable);
    // �ӽ�device�O����
		int *d_result;
		size_t pitch;
    // cudaMalloc(&d_result, in);
    cudaMallocPitch((void **)&d_result, &pitch, sizeof(int)*resX, (size_t) resY);
    
    // �w�qkernel������t�m
    dim3 blockSize(32, 32);
    dim3 gridSize((pitch/sizeof(float)+blockSize.x-1)/blockSize.x,(resY+blockSize.y-1)/blockSize.y);
    // ����kernel
    mandelKernel << < gridSize, blockSize >> >(d_result, lowerX, lowerY, stepX, stepY, maxIterations, resX, resY, pitch/sizeof(float));
    
    // �Ndevice�o�쪺���G������host
    cudaMemcpy2D(result, sizeof(float)*resX,d_result, pitch, sizeof(float)*resX, resY, cudaMemcpyDeviceToHost);

    memcpy(img, result, in);
    
    // ����device�O����
    cudaFree(d_result);
    // ����host�O����
    cudaFreeHost(result);
}
