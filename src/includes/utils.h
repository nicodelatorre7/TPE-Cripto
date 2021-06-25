#ifndef __UTILS__H__
#define __UTILS__H__

#include <stdint.h>
#include "bitmap.h"


int read_image_file(char * filename, BMPImage **image);

int read_images_directory(char *directory, BMPImage **all_images);

int write_image(BMPImage *image);

#endif /** __UTILS__H__ **/