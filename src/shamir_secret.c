#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shamir_secret.h"
#include <dirent.h>     /** https://pubs.opengroup.org/onlinepubs/007908775/xsh/dirent.h.html **/

/* 
 * https://www.codementor.io/@hbendali/c-c-macro-bit-operations-ztrat0et6
 * https://aticleworld.com/macros-for-bit-manipulation-c-cpp/
 */
#define CLEAR_BIT(x, pos) (x &= (~(1U << pos)))
#define SET_BIT(x, pos) (x |= (1U << pos))
#define CHECK_BIT(x, pos) (x & (1UL << pos) )
#define GET_BITS(x, pos) ((x & ( 1 << pos)) >> pos)


/**
 *   defines the structure dirent which includes the following members:
 *   ino_t  d_ino       file serial number
 *   char   d_name[]    name of entry
 **/ 
uint8_t get_n_of_bitmaps(char * directory){
    
    DIR * d;
    struct dirent * dir;
    d = opendir(directory);

    int n = 0;
    if(d){
        while ((dir = readdir(d)) != NULL){
            if (strstr(dir->d_name, ".bmp") != NULL){
                n +=1;
            }
        }
        closedir(d);
    }
    return n;
}


void swap_bit(uint8_t *x, uint8_t pos, uint8_t bit) {
    if (bit == 1) {
        SET_BIT(*x, pos);
    }
    else {
        CLEAR_BIT(*x, pos); 
    }
}

void swap_all(mat_representation * matrix, uint8_t bits[]) {
    
    swap_bit(&(matrix->w), 2, bits[7]);
    swap_bit(&(matrix->w), 1, bits[6]);
    swap_bit(&(matrix->w), 0, bits[5]);
    swap_bit(&(matrix->v), 2, bits[4]);
    swap_bit(&(matrix->v), 1, bits[3]);
    swap_bit(&(matrix->v), 0, bits[2]);
    uint8_t pad = bits[0]; 
    for (int b=1; b<8; b++) {
        pad ^= bits[b]; 
    }
    swap_bit(&(matrix->u), 2, pad);
    swap_bit(&(matrix->u), 1, bits[1]);
    swap_bit(&(matrix->u), 0, bits[0]);
}

int encrypt(char * filename, uint8_t k, char * directory){

    uint8_t n = get_n_of_bitmaps(directory);
    if (n < k) {
        printf("Error: Not enough images\n");
        exit(EXIT_FAILURE);
    }

    /** Cargo la imagen **/
    BMPImage *image;
    int r = read_image_file(filename, &image);
    if(r != 0){ // Si devuelve != 0 hubo algun error cargando fotos
        printf("Error loading photos\n");
        return EXIT_FAILURE;
    }

    printf("Foto secreta leída\n");

    /** CARGO TODAS LAS OTRAS FOTOS **/
    BMPImage *all_images[n]; 
    r = read_images_directory(directory, all_images);
    if (r != 0 ){ // Si devuelve != 0 hubo algun error cargando fotos
        printf("Error loading photos\n");
        return EXIT_FAILURE; 
    }
    printf("Todas las imagenes leídas\n");

    mat_representation * all_images_xwvu[n];
    size_t xwvu_size;
    for(size_t i = 0; i < n; i++ ){
        r = fill_matrix_array(all_images[i], &(all_images_xwvu[i]), &xwvu_size);
        if (r != 0 ){
            printf("Error reading photos body\n");
            return EXIT_FAILURE; 
        }   
    }

    printf("pasaje a matrix terminado\n");


    uint8_t result;
    int mat_index = 0;
    uint8_t used[256];
    uint8_t values[k]; 

    for(int i = 0 ; i < image->info_header.height_px * image->info_header.width_px ; i+=k){
        mat_index = (int) i/k;
        
        memset(used, 0, sizeof(used)/(sizeof(used[0])));
        memcpy(values, &(image->body)[i], k*sizeof(uint8_t));

        for(int j=0; j < n; j++){
            uint8_t aux = all_images_xwvu[j][mat_index].x_byte;
            while(used[aux]) {
                aux++;
            }
            all_images_xwvu[j][mat_index].x_byte =  aux;
            used[all_images_xwvu[j][mat_index].x_byte] = 1;

            // Evaluar el polinomio con galois
            result = evaluate_poly(values, k, all_images_xwvu[j][mat_index].x_byte);
            uint8_t bits[8] = {0};
            for (int b=0; b<8; b++) {
                bits[b] = GET_BITS(result, b);
            }
 
            swap_all(&(all_images_xwvu[j][mat_index]), bits);  
        }     
    }


    printf("pasamos la etapa de galua");

    /** Pasar de la matriz de xwuv a pixeles denuevo **/
    char * new_body; 
    for(int i=0; i < n; i++){
        fill_body_from_matrix(all_images[i], all_images_xwvu[i], xwvu_size, &new_body);

        /** TODO: Hacer los free del body **/

        all_images[i]->body = new_body;
        
        int written = write_image(all_images[i]);
        if( written != 0) {
            printf("Error writing image\n");
            return EXIT_FAILURE;
        }
    }

   return r;
}
