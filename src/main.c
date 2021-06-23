#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/shamir_secret.h"

#define ARGS_QTY 5
#define R "r"
#define D "d"
#define KMIN 4
#define KMAX 6


int main( int argc, char *argv[] )  {
   	if( argc == ARGS_QTY ) {
        if(strcmp(argv[1],D) != 0 && strcmp(argv[1],R) != 0 ){
            printf("First argument must be 'r' or 'd'\n");
            return EXIT_FAILURE;
        }
		char *imageSecret = argv[2];
        int k = strtol(argv[3], NULL, 10);
		if( k < KMIN || k > KMAX ){
            printf("k must be 4, 5 or 6\n");
            return EXIT_FAILURE;
		}

        char * directory = argv[4];

        if(strcmp(argv[1],D) == 0){
            encrypt(imageSecret,k,directory);
        }
        else {
            decrypt(imageSecret,k,directory);
        }
        
    } else if( argc > ARGS_QTY ) {
    	printf("Too many arguments supplied.\n");
   	}else {
        printf("Four arguments expected.\n");
  	}
}