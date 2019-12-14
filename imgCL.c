//  Copyright © 2019 Alan Vitullo. All rights reserved.
#define VERSION "Version 0.1"
//
//
#include <stdio.h>

//
//
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

//
// OpenCL Support Library
#include "utilsOpenCL.h"
// Image Support Libraries
#include "utilsJPEG.h"
#include "utilsPNG.h"
#include "utilsBMP.h"
// File/Dir Support Library
#include "utilsFLDR.h"

// 
#define MAX_SOURCE_SIZE (0x100000)

void addAlphaChannel();

//	---MAIN---
int main(int argc, char *argv[]) {

	//	--JPEG--
    	// Load JPEG Image data
	int jpeg_width, jpeg_height, jpeg_nrChannels;
	J_COLOR_SPACE jpeg_colormap;
	unsigned char * jpeg_data; 	
	char *jpeg_input_path = "images/Labrador-Retriever-MP.jpg";
	int  readJPEGerror;

	readJPEGerror = read_JPEG_file (&jpeg_width, &jpeg_height, &jpeg_nrChannels, & jpeg_colormap, &jpeg_data, jpeg_input_path, "false");

	int jpgIdx=0;

	int jpeg_pixelCount = jpeg_width* jpeg_height* jpeg_nrChannels; 
	unsigned char * jpeg_data_R = (unsigned char *)malloc(sizeof(unsigned char) * jpeg_pixelCount); 
	unsigned char * jpeg_data_G = (unsigned char *)malloc(sizeof(unsigned char) * jpeg_pixelCount); 
	unsigned char * jpeg_data_B = (unsigned char *)malloc(sizeof(unsigned char) * jpeg_pixelCount); 

  	for(int y= 0; y< jpeg_pixelCount; y+= jpeg_nrChannels) {

			// RED PX
			jpeg_data_R[y] = jpeg_data[jpgIdx];
			jpeg_data_R[y+1] = 0;
			jpeg_data_R[y+2] = 0;
			// GREEN PX
			jpeg_data_G[y] = 0;
			jpeg_data_G[y+1] = jpeg_data[jpgIdx+1];
			jpeg_data_G[y+2] = 0;
			//BLUE PX
			jpeg_data_B[y] = 0;
			jpeg_data_B[y+1] = 0;
			jpeg_data_B[y+2] = jpeg_data[jpgIdx+2];

			jpgIdx += jpeg_nrChannels;
  	}

	int newCh = jpeg_nrChannels+1;
	png_bytep *j2p_image = (png_bytep*)malloc(sizeof(png_bytep) * jpeg_height);
	
	//
	jpgIdx=0;

	for(int y = 0; y < jpeg_height; y++) {

		j2p_image[y] = (png_byte*)malloc(sizeof(png_byte) * (jpeg_width* newCh));

    		for(int x = 0; x < (jpeg_width* newCh); x+= newCh) {

			j2p_image[y][x] = jpeg_data_R[jpgIdx];
			j2p_image[y][x+1] = jpeg_data_R[jpgIdx+1];
			j2p_image[y][x+2] = jpeg_data_R[jpgIdx+2];
			j2p_image[y][x+3] = 255;

			jpgIdx += jpeg_nrChannels;
    		}

  	}

	// Save JPEG Image
	char * jpeg_output_path = "images/outJ.jpg";
	int jpeg_quality = 95;
	J_COLOR_SPACE color_space = JCS_RGB;

	char * jpeg_output_path_R = "images/outJ_R.jpg";
	char * jpeg_output_path_G = "images/outJ_G.jpg";
	char * jpeg_output_path_B = "images/outJ_B.jpg";

	write_JPEG_file (jpeg_width, jpeg_height, jpeg_nrChannels, color_space, jpeg_data_R, jpeg_quality, jpeg_output_path_R);
	write_JPEG_file (jpeg_width, jpeg_height, jpeg_nrChannels, color_space, jpeg_data_G, jpeg_quality, jpeg_output_path_G);
	write_JPEG_file (jpeg_width, jpeg_height, jpeg_nrChannels, color_space, jpeg_data_B, jpeg_quality, jpeg_output_path_B);

	write_JPEG_file (jpeg_width, jpeg_height, jpeg_nrChannels, color_space, jpeg_data, jpeg_quality, jpeg_output_path);

	//	--PNG--
	// Load PNG Image data
	int png_width=0; 
	int png_height=0;
	int png_nrChannels=0;
	png_byte * color_type = PNG_COLOR_TYPE_RGBA;

	png_bytep * png_data;
	char *preprocessing;
	char * png_input_path = "images/puppy.png";

	read_png_file(&png_width, &png_height, &png_nrChannels, color_type, &png_data, png_input_path, preprocessing);

	int png_pixelCount = png_width* png_height* png_nrChannels;

	//Processing

	// Save PNG Image
	char * png_output_path = "images/outP.png";
	int png_quality = 95;

	write_png_file(png_width, png_height, png_nrChannels, color_type, png_data, png_quality, png_output_path);

	char * j2p_output_path = "images/tenJPA.png";
	//write_png_file(jpeg_height, jpeg_width, jpeg_nrChannels, PNG_COLOR_TYPE_RGBA, j2p_image, jpeg_quality, j2p_output_path);

	//	--BMP--
	// Load BMP Image data
	BITMAPINFOHEADER bitmapInfoHeader;
    	unsigned char * bitmapData;
	char * bmp_input_path = "images/light_drops.bmp";

	read_bmp(&bitmapInfoHeader, &bitmapData, bmp_input_path);
    

    	// Load Image data
	int width, height, nrChannels;
	J_COLOR_SPACE colormap;
	unsigned char * data; 	/* allows batching with varying image types. eg, bmp/png/jpg */
	char *input_path = "images/outJ_B.jpg";


	readJPEGerror = read_JPEG_file (&width, &height, &nrChannels, &colormap, &data, input_path, "false");
	int pixelCount = width* height* nrChannels;

	float * dataFloat = (float *)malloc(pixelCount * sizeof(float *));

	for(int i=0; i< pixelCount; i++) {
		//printf("%u", data[i]);
		//printf("%f", ((float)data[i]/255.0) );
		dataFloat[i] = (float)data[i]/255.0;
	}



    int filterWidth = 3;
    int filterSize = filterWidth * filterWidth;
    static float filter[9] = {
	1.0f,3.0f,1.0f,
	3.0f,5.0f,3.0f,
	1.0f,3.0f,1.0f
    };
    
    FILE *fp;
    char *source_str;
    size_t source_size;
    fp = fopen("convolution_kernel.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );
    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1,
                         &device_id, &ret_num_devices);
    
    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
    
    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    
    // Create memory buffers on the device
    cl_image_format format;
    format.image_channel_order = CL_RGB;
    format.image_channel_data_type = CL_FLOAT;
    
    cl_mem bufferSourceImage = clCreateImage2D(
                                               context,
                                               0,
                                               &format,
                                               width,
                                               height,
                                               0,
                                               NULL,
                                               NULL
                                               );
    
    cl_mem bufferOutputImage = clCreateImage2D(
                                               context,
                                               0,
                                               &format,
                                               width,
                                               height,
                                               0,
                                               NULL,
                                               NULL
                                               );
    
    cl_mem bufferFilter = clCreateBuffer(
                                         context,
                                         0,
                                         filterSize* sizeof(float),
                                         NULL,
                                         NULL
                                         );

    cl_sampler sampler = clCreateSampler(context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, NULL);

    size_t origin[3] = {0,0,0};
    size_t region[3] = {width, height, 1};
    
    // Copy input image to memory buffer
    ret = clEnqueueWriteImage(command_queue, bufferSourceImage, CL_FALSE, origin, region, 0, 0, dataFloat, 0, NULL, NULL);

    // Copy filter to memory buffer
    ret = clEnqueueWriteBuffer(command_queue, bufferFilter, CL_FALSE, 0, filterSize* sizeof(float), filter, 0, NULL, NULL);
    
    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1,
                                                   (const char **)&source_str, (const size_t *)&source_size, &ret);
    
    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    
    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "convolution", &ret);
    
    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&bufferSourceImage);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufferOutputImage);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufferFilter);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&filterWidth);
    ret = clSetKernelArg(kernel, 4, sizeof(cl_sampler), (void *)&sampler);
    
    // Execute the OpenCL kernel on the list
    size_t global_item_size[2] = {width, height}; 
    size_t local_item_size[2] = {1,1}; 
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL,
                                 &global_item_size, &local_item_size, 0, NULL, NULL);
    
    // Read the memory buffer C on the device to the local variable C
    float * dataOutput = (float *)malloc(pixelCount * sizeof(float *));
    ret = clEnqueueReadImage(command_queue, bufferOutputImage, CL_TRUE, origin, region, 0, 0, dataOutput, 0, NULL, NULL);

	unsigned char * dataUchar = (unsigned char *)malloc(pixelCount * sizeof(unsigned char *));

	for(int i=0; i< pixelCount; i++) {
		//printf("%f", dataOutput[i]);
		//printf("%u", (unsigned char) dataOutput[i]*255.0);
		dataUchar[i] = ((unsigned char)(dataOutput[i]*255.0));
	}

	// Save Image
	char *output_path = "images/outCLJ.jpg";
	int quality = 95;

	write_JPEG_file (width, height, nrChannels, color_space, dataUchar, quality, output_path);
    
    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(bufferSourceImage);
    ret = clReleaseMemObject(bufferOutputImage);
    ret = clReleaseMemObject(bufferFilter);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);


return 0;
}
