#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "includes/utils.h"
#include "includes/bitmap.h"


int read_image_file(char * filename, BMPImage **image){
    FILE *fp = fopen(filename , "r" );
    if(fp == NULL) {
        printf("read_image_file: error opening file.");
        return -1;
    }
    return read_image(fp, image, filename);
    //int res = read_image(fp, image, filename);
    //printf("\nheight on read_image_file: %d\n", image->info_header.height_px);
    //return res;
}

int read_images_directory(char * directory, BMPImage **images){
    DIR * d;
    struct dirent * dir;
    d = opendir(directory);

    if(d){
        //BMPImage * aux = NULL;
        int n = 0;
        while ((dir = readdir(d)) != NULL){
            if (strstr(dir->d_name, ".bmp") != 0){
                char file[512];
                sprintf(file, "%s/%s",directory,dir->d_name);
                FILE *fp = fopen( file , "r" );
                if(fp == NULL) {
                    printf("ReadImagesDirectoryError: Error opening file.");
                    return -1;
                }
                //aux = (BMPImage *) realloc(aux, n * sizeof(BMPImage));
                if(read_image(fp, &(images[n]),file) != 0) {
                    printf("ReadImagesDirectoryError: Error reading image.\n");
                    return -1;
                }
                //printf("H: %d -- W: %d \n", aux[n].info_header.height_px, aux[n].info_header.width_px);
                n++;
            }
        }
        closedir(d);
        //printf("%d\n",n);
        
    } else {
        return -1;
    }

    return 0;
}

int write_image(BMPImage *image){
    //printf("file name > %s \n", image->filename);
    FILE *fp = fopen( image->filename , "wb" );
    if(fp == NULL) {
        printf("write_image: error opening file.\n");
        return -1;
    }

    //printf("offset> %d \n", image->header.data_offset);
    //printf("width> %d \n", image->info_header.width_px);
    //printf("height> %d \n", image->info_header.height_px);

    rewind(fp);
    fwrite(&(image->header), 1, sizeof(image->header), fp);
    fwrite(&(image->info_header), 1, sizeof(image->info_header), fp);
    //printf("written : %d \n", written);


    unsigned int untilOffsetl = image->header.data_offset - (sizeof(image->header) + sizeof(image->info_header) + 1 );

	fwrite(image->until_offset,  1, untilOffsetl , fp);

    fseek(fp, image->header.data_offset, SEEK_SET);
    //printf("PIXELS SIZE: %d \n",image->info_header.width_px*image->info_header.height_px);
    fwrite(image->body, 1, image->info_header.width_px*image->info_header.height_px, fp);
    //printf("written : %d \n", written);

    fclose(fp);
    return 0;
}
