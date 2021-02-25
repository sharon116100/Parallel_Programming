#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int status;
    cl_uint num_devices;
    cl_kernel kernel;
    cl_command_queue queue;
    int filterSize = filterWidth * filterWidth;
    
    /*
    status = clGetDeviceIDs (0, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
    printf(status);
		context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
		
		cl_platform_id platform;
    status = clGetPlatformIDs(1, &platform, NULL);
    CHECK(status, "clGetPlatformIDs");
    printf("HI status\n");*/
    queue = clCreateCommandQueue(*context, *device, (cl_command_queue_properties)0, &status);
    

    //CHECK(status, "clCreateCommandQueue");
    
    //create kernel
    kernel = clCreateKernel(*program, "convolution", &status);
    
    //CHECK(status, "clCreateKernel");
    
    //create image
    cl_image_format format;
    
    format.image_channel_order = CL_R;
    format.image_channel_data_type = CL_FLOAT;
    
    cl_mem input = clCreateImage2D(*context, CL_MEM_READ_ONLY, &format, imageWidth, imageHeight, 0, NULL, &status);
    cl_mem output = clCreateImage2D(*context, CL_MEM_WRITE_ONLY, &format, imageWidth, imageHeight, 0, NULL, &status);
    cl_mem bufferFilter = clCreateBuffer(*context, CL_MEM_READ_ONLY, filterSize*sizeof(float), NULL, NULL);
	  
		size_t origin[3];
	  origin[0] = 0;
	  origin[1] = 0;
	  origin[2] = 0;
    size_t region[3];
    region[0] = imageWidth;
    region[1] = imageHeight;
    region[2] = 1;
    
    status = clEnqueueWriteImage(queue, input, CL_TRUE, origin, region, 0, 0, inputImage, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufferFilter, CL_TRUE, 0, filterSize*sizeof(float), filter, 0, NULL, NULL);
    cl_sampler sampler = clCreateSampler(*context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, NULL);
    
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&input);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&output);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufferFilter);
    clSetKernelArg(kernel, 3, sizeof(cl_int), (void*)&filterWidth);
    clSetKernelArg(kernel, 4, sizeof(cl_sampler), (void*)&sampler);
    
    size_t globalws[2] = {imageWidth, imageHeight};
    size_t localws[2] = {8, 8};
    status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalws, NULL, 0, NULL, NULL);
    
    
    //return output
    clEnqueueReadImage(queue, output, CL_TRUE, origin, region, 0, 0, outputImage, 0, NULL, NULL);
    
    //free
    /*clReleaseMemObject(bufferFilter);
    clReleaseMemObject(input);
    clReleaseCommandQueue(queue);
    clReleaseKernel(kernel);*/
}