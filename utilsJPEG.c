#include "utilsJPEG.h"

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


/*	---READ---	*/
int read_JPEG_file (int * width, 
	int * height, 
	int * nrChannels, 
	J_COLOR_SPACE * colormap, 
	unsigned char ** data, 
	char *input_path, 
	char *preprocessing) {

  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;

  FILE * infile;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */

  if ((infile = fopen(input_path, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", input_path);
    return 0;
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);

	*width = cinfo.output_width;
	*height = cinfo.output_height;
	*nrChannels = cinfo.num_components;
	*colormap = cinfo.colormap;

	JSAMPLE * image_buffer;	/* Points to large array of R,G,B-order data */
	image_buffer = (JSAMPLE*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );

 /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;

  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	unsigned long location = 0;

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */
	printf("Reading image...\t%s\n", input_path);
	printf ("width: %3i height: %3i channels: %3i\n", cinfo.output_width, cinfo.output_height, cinfo.num_components);
	printf("format: %i pixels: %i\n", cinfo.colormap, cinfo.output_width*cinfo.output_height*cinfo.num_components);

  while (cinfo.output_scanline < cinfo.output_height) {

    (void) jpeg_read_scanlines(&cinfo, buffer, 1);

		for (int i=0; i < row_stride; i++){
			image_buffer[location++] = buffer[0][i];
			}
  }
	
	printf("Done scaning.\n");
	*data = image_buffer;

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);

  /* Step 8: Release JPEG decompression object */

  jpeg_destroy_decompress(&cinfo);

  fclose(infile);

  return 1;
}

/*	---WRITE---	*/
void write_JPEG_file (int width, int height, int nrChannels, J_COLOR_SPACE color_space, unsigned char * data, int quality, char *output_path) {

  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE * outfile;		/* target file */
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
  int row_stride;		/* physical row width in image buffer */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */
  if ((outfile = fopen(output_path, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", output_path);
    exit(1);
  }
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = width; 	/* image width and height, in pixels */
  cinfo.image_height = height;
  cinfo.input_components = nrChannels;		/* # of color components per pixel */
  cinfo.in_color_space = color_space; 	/* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = width * 3;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    row_pointer[0] = & data[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

}

