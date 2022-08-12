#ifndef BYTE_UTILITIES_H
#define BYTE_UTILITIES_H

#include <basic_utils.h>
#include <math.h>

void  unset_CAN_k_th_bit(__uint8_t v[],  __uint8_t k );
void  set_CAN_k_th_bit( __uint8_t v[],  __uint8_t k );
__uint8_t* float_to_byte_array(float f);
float byte_array_to_float(__uint8_t* v);
char float_to_byte(float val);
char short_to_byte(short val);
short float_to_short(float val);
bool is_bool(char* data_type);
__uint8_t bit_to_byte(__uint8_t bit_start);
void shift_array_left(__uint8_t* v, __uint8_t index, __uint8_t bits);
void bit_to_bit_AND_array(__uint8_t* v_dst, __uint8_t* v_src, __uint8_t len);
void bit_to_bit_OR_array(__uint8_t* v_dst, __uint8_t* v_src, __uint8_t len);

#endif //BYTE_UTILITIES_H