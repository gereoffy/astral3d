#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include "../jpeglib/jpeglib.h"

struct jpeg_decompress_struct cinfo;
struct jpeg_error_mgr jerr;
static unsigned char greypal[256][3];

int LoadJPG(FILE *input_file,map_st *map){
  unsigned char *buffer=NULL;
  int num_scanlines;
  int num_bytes;
  JSAMPROW jsamparray[1];
  
	cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  /* Specify data source for decompression */
  jpeg_stdio_src(&cinfo, input_file);

  /* Read file header, set default decompression parameters */
  (void) jpeg_read_header(&cinfo, TRUE);

  printf("JPG  %d x %d x %d\n",cinfo.image_width,cinfo.image_height,cinfo.num_components*8);
  buffer=malloc(cinfo.image_width*cinfo.image_height*cinfo.num_components);
  if(!buffer){ printf("Not enough memory\n");return 0;}
  
  /* Start decompressor */
  (void) jpeg_start_decompress(&cinfo);
  
  /* Process data */
  num_bytes=0;
  while (cinfo.output_scanline < cinfo.output_height) {
    jsamparray[0]=&buffer[num_bytes];
    num_scanlines=jpeg_read_scanlines(&cinfo, jsamparray, 1);
    num_bytes+=num_scanlines*cinfo.image_width*cinfo.num_components;
  }
  /* printf("total: %d bytes\n",num_bytes); */

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  map->xsize=cinfo.image_width;
  map->ysize=cinfo.image_height;
  map->map=buffer;
  map->colors=0;
  if(cinfo.num_components==1){    /* grayscaled */
    int i;
    for(i=0;i<256;i++) greypal[i][0]=greypal[i][1]=greypal[i][2]=i;
    map->colors=256;
    map->pal=&greypal[0][0];
  }
  
return 1;
}


