#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

__global__ void mandelKernel(int* result, float lowerX, float lowerY, float stepX, float stepY, int iter, int resX, int resY) {
    // To avoid error caused by the floating number, use the following pseudo code
    //
    int thisY = threadIdx.y + blockIdx.y * blockDim.y;
    int thisX = threadIdx.x + blockIdx.x * blockDim.x;
    int index = resX * thisY + thisX;
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
    result[index] = i;
    return;
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    int N = resX*resY;
		// 申請host記憶體
	  size_t in = N * sizeof(int);
    int* result = (int*)malloc(in);
    
    // 申請device記憶體
		int *d_result;
    cudaMalloc(&d_result, in);
    
    // 定義kernel的執行配置
    dim3 blockSize(16, 16);
    dim3 gridSize((resX + blockSize.x - 1) / blockSize.x, (resY + blockSize.y - 1) / blockSize.y);
    // 執行kernel
    mandelKernel << < gridSize, blockSize >> >(d_result, lowerX, lowerY, stepX, stepY, maxIterations, resX, resY);
    
    // 將device得到的結果拷貝到host
    cudaMemcpy(result, d_result, in, cudaMemcpyDeviceToHost);
    memcpy(img, result, in);
    
    // 釋放device記憶體
    cudaFree(d_result);
    // 釋放host記憶體
    free(result);
}
