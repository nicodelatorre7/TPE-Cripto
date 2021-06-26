#include <string.h>
#include <stdio.h>
#include "includes/math_helper.h"
#include <stdint.h>
#include <stdlib.h>

#define MAX 256
int p_interpolate(uint8_t * x, uint8_t * y, size_t k, uint8_t * polynom) {
/////variables
    uint8_t varup;
    uint8_t vardown;
    uint8_t aux;
        ///arrays
    uint8_t L0[k];
    uint8_t s[k];

    memset((void *) L0, 0, k*sizeof(uint8_t));
    memset((void *) s, 0, k*sizeof(uint8_t));
    int sum;
    uint8_t acum;
    uint8_t v_a[k];
    memcpy(v_a, x, k*sizeof(x[0]));
    //helpers
    uint8_t v_b[k];
    memcpy(v_b, y, k*sizeof(y[0]));
    uint8_t var_a, var_b;
    
    for (size_t r = 0; r < k; r++) {
        if (v_a[r] == 0) {
            var_a = v_a[0];//sprintf("debug")
            //////////////////// hasta aca bien
            var_b = v_b[0];
            v_a[0] = v_a[r];
            v_b[0] = v_b[r];

            v_a[r] = var_a;
            //sprintf("punto A")
            v_b[r] = var_b;
            break;
        }
    }
    for(size_t j=0; j<k-1; j++) {
        for(size_t i =j; i < k; i++){
            sum = 1;
            acum = 0;

            for(size_t h = j; h < k; h++){
                if (h != i) {
                //sprintf("debug")
                    varup = substrate(0, v_a[h]);
                    vardown = substrate(v_a[i], v_a[h]);
                    aux = divide(varup, vardown);
                    if (sum) {
                                //sprintf("punto B")
                        sum = 0;
                        acum = aux;
                    }
                    else {
                        acum = multiply(acum, aux);
                    }
                }
            }
            L0[i] = acum;
        }
        sum = 1;
        acum = 0;
        for(size_t i=j; i<k; i++) {
            aux = multiply(L0[i], v_b[i]);
            if (sum) {
                sum = 0;
                acum = aux;
                //sprintf("debug")
            }
            else {
                acum = add(acum, aux);
            }
        }
        s[j] = acum;

       ////////ask a juan
        for(size_t i=j+1; i<k; i++) {
            varup = substrate(v_b[i], s[j]);
            vardown = v_a[i];
            aux = divide(varup, vardown);
            v_b[i] = aux;
        }
    }
    s[k-1] = v_b[k-1];
    memcpy(polynom, s, k * sizeof(polynom[0]));
    return 0;
}


int p_evaluate(uint8_t * polynom, size_t poly_g, uint8_t x, uint8_t * result) {
        uint8_t res = polynom[0];
    if (poly_g > 1) {
       uint8_t x_term = x;
       res = add(res, multiply(polynom[1], x_term));
        for(size_t i = 2; i < poly_g ; i++) {
            x_term = multiply(x_term, x);
            res = add(res, multiply(polynom[i], x_term));
      }
    }

    *result = res;

    return 0;
}

static uint16_t PRIMITIVE_GENERATIVE_POLYNOMIAL= 0x163;
static uint8_t multiplication_t[MAX][MAX];
static uint8_t inverse_multiplication_t[MAX];

/////////////

static uint8_t helper(uint8_t a, uint8_t b) {
	uint8_t rta = 0;
	while (a && b) {
         if (b & 1){
            rta ^= a;}
         if (a & 128){
            a = (a << 1) ^ PRIMITIVE_GENERATIVE_POLYNOMIAL;}
         else
            a = a*2;
        b >>= 1;
	}
	return rta;
}

static uint8_t get_polynom_degree(uint8_t p) {
    p >>= 1;    
    uint8_t rta = 0;
    while (p != 0) {
        rta++;p>>=1;
        }
        return rta;
}

static uint8_t invertion_helper(uint16_t p, uint16_t s) {
    uint16_t inv = s;
    uint16_t aux;

    int fd = get_polynom_degree(p) - 8;

    uint16_t var1 = 1;
    uint16_t var2 = 0;

    while (p != 1) {

        if (fd < 0) {

            //check
            aux = inv;
            inv = p;
            p = aux;
            aux = var1;
            var1 = var2; //TODO
            var2 = aux;
               //fix bug que vio nico
            fd = -fd;}
        p ^= inv << fd;
        var1 ^= var2 << fd;
        //find
        p %= MAX;
        var1 %= MAX;

        fd = get_polynom_degree(p) - get_polynom_degree(inv);
    }
    return (uint8_t)var1;
}

////////// recheck helper
int matrix_initialization() {
    for (int i=1; i<MAX; i++) {
        for (int j=1; j<MAX; j++) {
            multiplication_t[i-1][j-1] = helper(i, j);
        }
    }
    for (int i=1; i<MAX; i++) {
        inverse_multiplication_t[i-1] = invertion_helper(i, PRIMITIVE_GENERATIVE_POLYNOMIAL);
    }
    return 0;
}

uint8_t add(uint8_t a, uint8_t b) {
	return a ^ b;
}

uint8_t multiply(uint8_t a, uint8_t b) {
    if( a == 0|| b== 0 ){ return 0;}
        return multiplication_t[a-1][b-1];
}

////
uint8_t substrate(uint8_t a, uint8_t b) {
	return add(a, b);}

uint8_t divide(uint8_t a, uint8_t b) {
    if (a== 0 || b ==0 ) { return 0; }
    uint8_t inverse_multiplication_aux = inverse_multiplication_t[b-1];
	return multiply(a, inverse_multiplication_aux);
}


