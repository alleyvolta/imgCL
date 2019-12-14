//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>
//
#include "jpeglib.h"

#ifndef UTILSJPEG
#define UTILSJPEG

#ifdef __cplusplus
extern "C" {
#endif

int read_JPEG_file (int * width, int * height, int * nrChannels, J_COLOR_SPACE * colormap, unsigned char ** data, char *input_path, char *preprocessing);

void write_JPEG_file (int width, int height, int nrChannels, J_COLOR_SPACE color_space, unsigned char * data, int quality, char *output_path);

#ifdef __cplusplus
}
#endif

#endif 
