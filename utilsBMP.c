//
#include "utilsBMP.h"

int read_bmp(BITMAPINFOHEADER *bitmapHeader, unsigned char ** data,  char * input_path) {
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable
    
    //open filename in read binary mode
    filePtr = fopen(input_path,"rb");
    if (filePtr == NULL)
        return 9;
    
    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);
    
    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(filePtr);
        return 67;
    }
    
    //read the bitmap info header
    fread(bitmapHeader, sizeof(BITMAPINFOHEADER),1,filePtr); // small edit. forgot to add the closing bracket at sizeof
    
    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
    
    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapHeader->biSizeImage);
    
    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return 11;
    }
    
    //read in the bitmap image data
    fread(bitmapImage, bitmapHeader->biSizeImage,1,filePtr);
    
    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return 66;
    }
    
    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapHeader->biSizeImage;imageIdx+=3) // fixed semicolon
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }
    
    //close file and return bitmap iamge data
    fclose(filePtr);

	*data = bitmapImage;
    return 0;
}




int write_bmp(BITMAPINFOHEADER *bitmapHeader, unsigned char * data, char *output_path) {

    FILE *fp = fopen(output_path, "wb");
    // Write header
    rewind(fp);
    int num_read = fwrite(bitmapHeader, sizeof(bitmapHeader), 1, fp);
    if (num_read != 1) {
	printf("Cannot write header.\n");
        return 9;
    }
    // Write image data
    num_read = fwrite(data, bitmapHeader->biSizeImage, 1, fp);
    if (num_read != 1) {
	printf("Cannot write image.\n");
        return 9;
    }
    
    return 0;
}


