#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#pragma pack(1)
typedef struct {
    uint16_t signature;         // 'BM'
    uint32_t size;              // File size in bytes
    uint32_t reserved;          // unused (=0)
    uint32_t data_offset;       // Offset from beginning of file to the beginning of the bitmap data
} Header;

#pragma pack(1)
typedef struct {
    uint32_t info_header_size;  // Size of InfoHeader =40 
    int32_t  width_px;          // Horizontal width of bitmap in pixels
    int32_t  height_px;         // Vertical height of bitmap in pixels
    uint16_t planes;        // Number of Planes (=1)
    uint16_t bits_per_pixel;    // Bits per pixel
    uint32_t compression_type;       // Type of Compression  
    uint32_t image_size;  // (compressed) Size of Image
    int32_t  x_resolution;  // horizontal resolution: Pixels/meter
    int32_t  y_resolution;  // vertical resolution: Pixels/meter
    uint32_t num_colors;        // Number of actually used colors.
    uint32_t num_colors_important;  // Number of important colors 
} InfoHeader;

typedef struct {
    char filename[256];
    Header header;
    InfoHeader info_header;
    uint8_t* until_offset;
    uint8_t* body;
} BMPImage;


typedef struct mat_representation {
    uint8_t x_byte;
    uint8_t w_byte;
    uint8_t v_byte;
    uint8_t u_byte;
} mat_representation;


int read_image(FILE *fp, BMPImage **image, char * filename);

int fill_matrix_array(BMPImage *image, mat_representation ** mat_array, size_t * arr_size);

int fill_body_from_matrix(BMPImage *image, mat_representation *mat_array, size_t arr_size, uint8_t  ** body);


#endif