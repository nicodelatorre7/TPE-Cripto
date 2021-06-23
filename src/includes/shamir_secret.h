#ifndef __SHAMIR_SECRET__H__
#define __SHAMIR_SECRET__H__
#include <stdint.h>

int encrypt(char * filename, uint8_t k, char * directory);

int decrypt(char * filename, uint8_t k, char * directory);

#endif /** __SHAMIR_SECRET__H__ **/

