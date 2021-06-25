#include <stdio.h>
#include <string.h>
#include "includes/shamir_secret.h"
#include "includes/math_helper.h"
// El include de los bitmaps
#include <dirent.h>

/* 
 * https://www.codementor.io/@hbendali/c-c-macro-bit-operations-ztrat0et6
 * https://aticleworld.com/macros-for-bit-manipulation-c-cpp/
 */
#define CLEAR_BIT(x, pos) (x &= (~(1U << pos)))
#define SET_BIT(x, pos) (x |= (1U << pos))
#define CHECK_BIT(x, pos) (x & (1UL << pos) )
#define GET_BITS(x, pos) ((x & ( 1 << pos)) >> pos)



/***** HELPERS FUNCTIONS *****/


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
    
    swap_bit(&(matrix->w_byte), 2, bits[7]);
    swap_bit(&(matrix->w_byte), 1, bits[6]);
    swap_bit(&(matrix->w_byte), 0, bits[5]);
    swap_bit(&(matrix->v_byte), 2, bits[4]);
    swap_bit(&(matrix->v_byte), 1, bits[3]);
    swap_bit(&(matrix->v_byte), 0, bits[2]);
    uint8_t pad = bits[0]; 
    for (int b=1; b<8; b++) {
        pad ^= bits[b]; 
    }
    swap_bit(&(matrix->u_byte), 2, pad);
    swap_bit(&(matrix->u_byte), 1, bits[1]);
    swap_bit(&(matrix->u_byte), 0, bits[0]);
}

void get_all_bits(mat_representation m, uint8_t *bits, uint8_t *x, uint8_t *y, size_t *image_counter){
    uint8_t w_aux = m.w_byte;
    uint8_t v_aux = m.v_byte;
    uint8_t u_aux = m.u_byte;
    size_t counter_aux = *image_counter;
    bits[7] = GET_BITS(w_aux, 2);
    bits[6] = GET_BITS(w_aux, 1);
    bits[5] = GET_BITS(w_aux, 0);
    bits[4] = GET_BITS(v_aux, 2);
    bits[3] = GET_BITS(v_aux, 1);
    bits[2] = GET_BITS(v_aux, 0); 
    bits[1] = GET_BITS(u_aux, 1);
    bits[0] = GET_BITS(u_aux, 0);

    uint8_t pad = 0; 
    for (int b=0; b<8; b++) {
        pad ^= bits[b]; 
    }
    pad = GET_BITS(pad, 0);

    uint8_t read_pad = 0;
    read_pad = GET_BITS(u_aux, 2);

    if (pad == read_pad) {
        x[counter_aux] = m.x_byte;
        y[counter_aux] = 0;
        for (int b=0; b<8; b++) {
            swap_bit(&(y[counter_aux]), b, bits[b]); 
        }
        counter_aux++;
    }

    *image_counter = counter_aux;
}
/***** End of HELPERS FUNCTIONS *****/


/*** FREE'S **/
void free_matrix(mat_representation * m) {
    free(m);
}

void free_single_image(BMPImage * image) {
    free(image->body);
    free(image->until_offset);
    free(image);
}

void free_all_images(BMPImage ** all_images, int n) {
    for (int i=0; i < n ; i++) {
        free_single_image(all_images[i]);
    }
}
/*** End of FREE'S **/


int encrypt(char * filename, uint8_t k, char * directory){
    
    uint8_t n = get_n_of_bitmaps(directory);
    if (n < k) {
        printf("Error: Not enough images\n");
        exit(EXIT_FAILURE);
    }

    /** Cargo la imagen **/   
    BMPImage * image;
    int r = read_image_file(filename, &image);
    if (r != 0) {
        printf("Error loading secret photo\n");
        exit(EXIT_FAILURE);
    }

    /** CARGO TODAS LAS OTRAS FOTOS **/
    BMPImage * all_images[n]; 
    r = read_images_directory(directory, all_images); 
    if (r != 0 ){
        printf("Error loading directory photos\n");
        exit(EXIT_FAILURE);
    }

    size_t size;
    mat_representation * m_aux;
    mat_representation * all_images_matrix[n];

    for(size_t i = 0; i < n; i++ ){
        r = fill_matrix_array(all_images[i],&m_aux,&size);
        if (r != 0 ){
            printf("Error reading photos pixels\n");
            exit(EXIT_FAILURE);
        }   
        all_images_matrix[i] = m_aux;
    }

    uint8_t result;
    int mat_index = 0;
    uint8_t used[256];
    uint8_t values[k]; 

    for(size_t i = 0; i < (size_t) (image->info_header.height_px * image->info_header.width_px) ; i+=k){
        mat_index = (int) i/k;
        memset(used, 0, sizeof(used)/(sizeof(uint8_t)));
        memcpy(values, &image->body[i], k*sizeof(uint8_t)); 

        for(size_t j=0; j < n; j++){            
            uint8_t aux = all_images_matrix[j][mat_index].x_byte;
            while(used[aux]) {
                aux++;
            }
            all_images_matrix[j][mat_index].x_byte =  aux;
            used[all_images_matrix[j][mat_index].x_byte] = 1;
            uint8_t bits[8] = {0};
            r = p_evaluate(values, k, all_images_matrix[j][mat_index].x_byte, &result); 
            if (r != 0 ){
                printf("Error in poly evaluation\n");
                exit(EXIT_FAILURE); 
            }

            for (int b=0; b<8; b++) {
                bits[b] = GET_BITS(result, b);
            }

            swap_all(&(all_images_matrix[j][mat_index]), bits);
    
        }     
    }

    /** Pasar de la matriz a pixeles denuevo **/
    uint8_t * new_body; 
    for(size_t i=0; i < n; i++){
        fill_body_from_matrix(all_images[i], all_images_matrix[i], size, &new_body);
        free(all_images_matrix[i]);
        free(all_images[i]->body);
        all_images[i]->body = new_body;
        int written = write_image(all_images[i]);
        if( written != 0) {
            printf("Error writing image\n");
            exit(EXIT_FAILURE); 
        }
    }

    /** Libero la que encripte **/
    free_single_image(image);
    /** Libero todas las otras imagenes **/
    free_all_images(all_images, n);

    return r;
}

int decrypt(char * filename, uint8_t k, char * directory){
    
    uint8_t n = get_n_of_bitmaps(directory);
    if (n < k) {
        printf("Error: Not enough images\n");
        exit(EXIT_FAILURE);
    }    

    /** CARGO TODAS LAS OTRAS FOTOS **/
    BMPImage *all_images[n]; 
    int r = read_images_directory(directory, all_images);
    if (r != 0) {
        printf("Error loading photos\n");
        return EXIT_FAILURE;
    }

    size_t size;
    mat_representation * m_aux;
    mat_representation * all_images_matrix[n];  
    for(size_t i = 0; i < n; i++ ){
        r = fill_matrix_array(all_images[i],&m_aux,&size);
        if (r != 0 ){
            return 1; 
        }   
        all_images_matrix[i] = m_aux;
    }

    /** Variables para Lagrange **/
    uint8_t values[k];
    uint8_t x[k];
    uint8_t y[k];
    size_t length = (all_images[0]->info_header.width_px * all_images[0]->info_header.height_px)/k;
    size_t image_counter = 0;
    uint8_t body[k*size];

    for(size_t i = 0; i < length; i++){
        image_counter = 0;
        for(size_t img = 0; (image_counter < k) && (img < n); img++){  
            uint8_t bits[8] = {0};
            uint8_t reader = 0;
            uint8_t p = 0;
            get_all_bits(all_images_matrix[img][i],bits,x,y,&image_counter);
        }

        if (image_counter == k) {
            r = p_interpolate(x, y, k, values);
            if (r != 0 ){
                return 1; 
            }
        }
        else {
            memset(values, 0, k * sizeof(uint8_t));
        }
        memcpy(&(body[i*k]), values, k * sizeof(uint8_t));
    }

    for(size_t i = 0 ; i < n; i++){
        free(all_images_matrix[i]);
    }


    /** Armo la nueva foto **/
    BMPImage new_image;
    memcpy(&new_image, all_images[0], sizeof(BMPImage));
    new_image.body = body;
    size_t len = strlen(filename);
    memcpy(new_image.filename, filename, len);
    new_image.filename[len] = '\0';
    int written = write_image(&new_image);
    free_all_images(all_images, n);

    return r; 
}
