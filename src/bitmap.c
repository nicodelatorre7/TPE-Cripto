#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "includes/bitmap.h"
#include "includes/utils.h"




int read_image(FILE *fp, BMPImage **image, char * filename){

    *image = malloc(sizeof(BMPImage));
    if(*image == NULL) {
        printf("LoadImageError: Malloc error.\n");
        return -1;
    }
    //printf("1: %s\n",filename);
    //printf("1: %d\n",strlen(filename));
    memset((*image)->filename, '\0', sizeof((*image)->filename));
    memcpy((*image)->filename,filename,strlen(filename));
    //strcpy((*image)->filename, filename);
    //printf("2: %s\n",(*image)->filename);
    
    // header
    rewind(fp);
    int header_num_read = fread(&((*image)->header), 1, sizeof((*image)->header), fp);
    //printf("header_num_read: %d\n", header_num_read);
    if(header_num_read != sizeof((*image)->header)) {
        printf("Load_image: error reading bmp header.\n");
        return -1;
    }
    int header_info_num_read = fread(&((*image)->info_header), 1, sizeof((*image)->info_header), fp);
    //printf("header_info_num_read: %d\n", header_info_num_read);
    if(header_info_num_read != sizeof((*image)->info_header)) {
        printf("Load_image: error reading bmp info header.\n");
        return -1;
    }

    //printf("image offset: %u \n", (*image)->header.data_offset);
    //printf("image width: %u \n", (*image)->header.reserved);
    //printf("image width: %u \n", (*image)->info_header.width_px);
    //printf("image height: %u \n", (*image)->info_header.height_px);
    int32_t image_size = ((*image)->info_header.width_px)* ((*image)->info_header.height_px);

    int untilOffsetl = (*image)->header.data_offset - (sizeof((*image)->header) + sizeof((*image)->info_header));
    //printf("UNTIL NUMBER SIZE: %d\n", untilOffsetl);
    (*image)->until_offset = malloc(sizeof(uint8_t) * untilOffsetl);

    int untilOffset_num_read = fread((*image)->until_offset, 1, untilOffsetl, fp);
    //printf("untilOffset_num_read: %d\n", untilOffset_num_read);
    if(untilOffset_num_read != untilOffsetl) {
        printf("Load_image: error reading data until offset.\n");
        return -1;
    }

    // image body
    fseek(fp, (*image)->header.data_offset, SEEK_SET);
    (*image)->body = malloc(sizeof(uint8_t) * image_size);
    if ((*image)->body == NULL){
        printf("Load_image: error allocating body\n");
        return -1;
    }

    //printf("image size: %d\n", image_size);
    //printf("Body read:\n");

    // Read image data
    fread((*image)->body, 1, image_size, fp);
    //printf("num_read: %d\n", num_read);
    /*if (num_read != image_size){
        printf("Load_image: error reading image data\n");
        return -1;
    }*/

    //print_image((*image)->body, (*image)->info_header.width_px, (*image)->info_header.height_px);

    fclose(fp);

    return 0;
}


int fill_body_from_matrix(BMPImage *image , mat_representation *mat_array, size_t arr_size, uint8_t  ** body){
    int32_t image_width = image->info_header.width_px;
    int32_t image_height = image->info_header.height_px;

    int size = arr_size*4;
    *body = malloc(sizeof(uint8_t) * size);
    if( *body == NULL ) {
        return -1;
    }

    for (int i=0; i < (int) arr_size; i++) {

        (*body)[(2 *(image_height/2 - ( i / (image_height/2) )) - 1)* 2*(image_width/2) + 2*( i % (image_width/2) )] = mat_array[i].x_byte;

        (*body)[(2 *(image_height/2 - ( i / (image_height/2) )) - 1)* 2*(image_width/2) + (2*( i % (image_width/2) ) + 1)] = mat_array[i].w_byte;

        (*body)[((2 *(image_height/2 - ( i / (image_height/2) )) - 1) - 1)* 2*(image_width/2) + 2*( i % (image_width/2) )] = mat_array[i].v_byte;

        (*body)[((2 *(image_height/2 - ( i / (image_height/2) )) - 1) - 1)* 2*(image_width/2) + (2*( i % (image_width/2) ) + 1)] = mat_array[i].u_byte;

    }


    return size;
}


int fill_matrix_array(BMPImage *image, mat_representation ** mat_array, size_t * arr_size){
    uint8_t *body = image->body;
    int32_t image_width = image->info_header.width_px;
    int32_t image_height = image->info_header.height_px;
    int count = 0;
    *arr_size = image_height*image_width / 4;
    

    *mat_array = malloc(sizeof(mat_representation) * (*arr_size) );
    

    uint8_t body_matrix[image_height][image_width];


    for(int c = image_height*image_width -1 ; c>=0; c--) {
        int row = (c / image_height);
        int col = c % image_width;
        body_matrix[row][col] = body[c];
    }

    for(int rowA = image_height-1; rowA > 0; rowA -= 2) {
        int rowB = rowA - 1;
        int squares = image_width / 2;
        
        int offset = 0;
        while( squares > 0 ) {
            (*mat_array)[count].x_byte = body_matrix[rowA][offset];
            (*mat_array)[count].w_byte = body_matrix[rowA][offset+1];
            (*mat_array)[count].v_byte = body_matrix[rowB][offset];
            (*mat_array)[count].u_byte = body_matrix[rowB][offset+1];

            offset += 2;
            count++;
            squares--;
        }
    }
    return 0;
}

