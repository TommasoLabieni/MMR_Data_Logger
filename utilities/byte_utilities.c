#include <byte_utilities.h>

/* Function that set k-th bit to 0 */
void  unset_CAN_k_th_bit(__uint8_t v[],  __uint8_t k )
{
    v[k/8] &= ~(1 << (k%8));
}
/* Function that set k-th bit to 1 */
void  set_CAN_k_th_bit( __uint8_t v[],  __uint8_t k )
{
    v[k/8] |= 1 << (k%8);
}

__uint8_t* float_to_byte_array(float f) {
    __uint8_t* ret = malloc(4 * sizeof(__uint8_t));
    unsigned int asInt = *((int*)&f);

    int i;
    for (i = 0; i < 4; i++) {
        ret[i] = (asInt >> 8 * i) & 0xFF;
    }

    return ret;
}

float byte_array_to_float(__uint8_t* v)
{
    int temp, start = 0, end = 3;
    /* Reverse array. TODO: Understant why a float is in BIG ENDIAN ?????? */
    while (start < end)
    {
        temp = v[start];  
        v[start] = v[end];
        v[end] = temp;
        start++;
        end--;
    }  
    return *(float *) v;
}

/* !!!!!! ASSERTION: WHEN CONVERTING FLOAT/SHORT TO BYTE THE MAXIMUM VALUE IS 2^8 - 1 == 255 !!!!!! */

char float_to_byte(float val)
{
    int x = round(val);

    return x & 0xFF;
}

char short_to_byte(short val)
{
    return val & 0xFF;
}

short float_to_short(float val)
{
    int x = round(val);

    return x & 0xFFFF;
}

__uint8_t bit_to_byte(__uint8_t bit_start)
{
    return bit_start / 8;
}

void shift_array_left(__uint8_t* v, __uint8_t index, __uint8_t bits)
{
    v[index] <<= bits;
}

void bit_to_bit_AND_array(__uint8_t* v_dst, __uint8_t* v_src, __uint8_t len)
{
    for (__int8_t i=0; i < len; i++)
    {
        v_dst[i] &= v_src[i];
    }
}

void bit_to_bit_OR_array(__uint8_t* v_dst, __uint8_t* v_src, __uint8_t len)
{
    for (__int8_t i=0; i < len; i++)
    {
        v_dst[i] |= v_src[i];
    }
}