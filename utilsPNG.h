#include <stdlib.h>
#include <stdio.h>
#include <png.h>

#ifndef UTILSPNG
#define UTILSPNG

int read_png_file(int * width, int * height, int * nrChannels, png_byte * color_type, unsigned char ** data, char *input_path, char *preprocessing);

int write_png_file(int width, int height, int nrChannels, png_byte * color_type, png_bytep * data, int quality, char *output_path);

int jpg2png(int height, int width, int nrChannels, png_byte * color_type, unsigned char * data, png_bytep ** row_pointers);

void print_png_pixels(int height, int width, png_bytep * row_pointers);

#endif