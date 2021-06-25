#ifndef __UTILS__H__
#define __UTILS__H__

#include <stdint.h>
#include "bitmap.h"

typedef struct mat_representation {
    uint8_t x_byte;
    uint8_t w_byte;
    uint8_t v_byte;
    uint8_t u_byte;
} mat_representation;


int read_image_file(char * filename, BMPImage **image);

int read_images_directory(char *directory, BMPImage **all_images);

int write_image(BMPImage *image);

#endif /** __UTILS__H__ **/