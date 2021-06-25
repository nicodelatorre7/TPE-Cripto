#ifndef ___MATH_HELPER_H___
#define ___MATH_HELPER_H___

#include <stdint.h>
#include <stdlib.h>

int p_interpolate(uint8_t * x, uint8_t * y, size_t k, uint8_t * polynom);
int p_evaluate(uint8_t * polynom, size_t poly_g, uint8_t x, uint8_t * result);

int matrix_initialization();
uint8_t add(uint8_t a, uint8_t b);
uint8_t multiply(uint8_t a, uint8_t b);
uint8_t substrate(uint8_t a, uint8_t b);
uint8_t divide(uint8_t a, uint8_t b);
int evaluate_poly(uint8_t * p, size_t grade, uint8_t x);

#endif /** ___MATH_HELPER_H___ **/

