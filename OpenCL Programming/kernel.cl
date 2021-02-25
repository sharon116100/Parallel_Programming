__kernel void convolution(__read_only image2d_t inputImage, __write_only image2d_t outputImage, __constant float* filter, int filterWidth, sampler_t sampler) 
{
    int column = get_global_id(0);
		int row = get_global_id(1);
		int halfWidth = (int) (filterWidth/2);
    float4 sum = {0.0f, 0.0f, 0.0f, 0.0f};
					 
    int filterIndex = 0;
		
		int2 coords;
		
		for(int i=-halfWidth; i<=halfWidth; i++)
		{
		   coords.y = row+i;
		   for(int j=-halfWidth; j<=halfWidth; j++)
		   {
		       coords.x = column+j;
		       float4 pixel;
		       pixel = read_imagef(inputImage, sampler, coords);
		       sum.x += pixel.x * filter[filterIndex++];
			 }
		}
		coords.x = column;
		coords.y = row;
		write_imagef(outputImage, coords, sum);
															
}

