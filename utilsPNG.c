#include "utilsPNG.h"

int read_png_file(int * width, int * height, int * nrChannels, png_byte * color_type, unsigned char ** data, char *input_path, char *preprocessing) {

  FILE *fp = fopen(input_path, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  int png_width      = png_get_image_width(png, info);
  int png_height     = png_get_image_height(png, info);
  int png_color_type = png_get_color_type(png, info);
  int png_channels = png_get_channels(png, info);

printf("h:%i w:%i \n", png_height, png_width);

  *width = png_width;
  *height = png_height;
  *nrChannels = png_channels;
  color_type = png_color_type;

  png_byte bit_depth;
  bit_depth  = png_get_bit_depth(png, info);

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(bit_depth == 16)
    png_set_strip_16(png);

  if(png_color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(png_color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  if(png_color_type == PNG_COLOR_TYPE_RGB ||
     png_color_type == PNG_COLOR_TYPE_GRAY ||
     png_color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(png_color_type == PNG_COLOR_TYPE_GRAY ||
     png_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  png_bytep *row_pointers = NULL;

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * png_height);

  for(int y = 0; y < png_height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
}

  png_read_image(png, row_pointers);

  *data = row_pointers;

  fclose(fp);

  png_destroy_read_struct(&png, &info, NULL);

return 0;
}

int write_png_file(int width, int height, int nrChannels, png_byte * color_type, png_bytep * data, int quality, char *output_path) {

  FILE *fp = fopen(output_path, "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth
  png_set_IHDR(
    png,
    info,
    width, height,
    8,
    color_type,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);

//print_png_pixels(height, width, data);

  if (!data) abort();

  png_write_image(png, data);
  png_write_end(png, NULL);

  fclose(fp);

  png_destroy_write_struct(&png, &info);

return 0;
}

int jpg2png(int height, int width, int nrChannels, png_byte * color_type, unsigned char * data, png_bytep ** row_pointers) {

  for(int y = 0; y < height; y++) {
	//printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
	*row_pointers[y] = data[y*nrChannels*width];

  }

return 0;
}

/*
int jpg2png(int height, int width, int nrChannels, png_byte * color_type, unsigned char * data, png_bytep ** row_pointers) {

	int Idx;

  for(int y = 0; y < height; y++) {

    png_bytep row = data[y];

    for(int x = 0; x < width; x++) {

      png_bytep px = &(row[x * 4]);

      // Do something awesome for each pixel here...
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
	*row_pointers[Idx] = px[0];
	Idx++;
	*row_pointers[Idx] = px[1];
	Idx++;
	*row_pointers[Idx] = px[2];
	Idx++;
    }
  }

return 0;
}
*/

//DEBUG FUNCTION, NOT FOR PRODUCTION
void print_png_pixels(int height, int width, png_bytep * row_pointers) {

  for(int y = 0; y < height; y++) {

    png_bytep row = row_pointers[y];

    for(int x = 0; x < width; x++) {

      png_bytep px = &(row[x * 4]);

      // Do something awesome for each pixel here...
      printf("px: %4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);

    }
  }

}

