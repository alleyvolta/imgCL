__kernel void convolution(__read_only image2d_t inputImage,
	__write_only image2d_t outputImage,
	constant float* filter,
	int filterWidth,
	sampler_t sampler
)

{
	int column = get_global_id(0);
	int row = get_global_id(1);

	//printf("%i , %i\n", column, row);

	int halfWidth = (int)(filterWidth/2);

	float4 sum = {0.f, 0.f, 0.f, 0.0f};

	int filterIdx = 0;

	int2 coords;

	for(int i = -halfWidth; i <= halfWidth; i++) {
		coords.y = row + i;
		
		for(int j = -halfWidth; j <= halfWidth; j++) {
			coords.x = column + j;

			float4 pixel;
			pixel = read_imagef(inputImage, sampler, coords);
			//printf("x%f y%f z%f w%f\n", pixel.x, pixel.y, pixel.z, pixel.w);
			sum.x += pixel.x * filter[filterIdx++];
			sum.y += pixel.y * filter[filterIdx++];
			sum.z += pixel.z * filter[filterIdx++];
		}
	}
	//printf("%v4f", sum);
	coords.x = column;
	coords.y = row;
	write_imagef(outputImage, coords, sum);
}

__kernel void convolution2(__read_only image2d_t inputImage,
	__write_only image2d_t outputImage,
	constant float* filter,
	int filterWidth,
	sampler_t sampler
)

{
	int column = get_global_id(0);
	int row = get_global_id(1);

	//printf("%i , %i\n", column, row);
	int2 coords;

	coords.x = column;
	coords.y = row;

	float4 pixel;
	pixel = read_imagef(inputImage, sampler, coords);
	//printf("%v4f", pixel);

	write_imagef(outputImage, coords, pixel);
}
