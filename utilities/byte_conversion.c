#include <byte_conversion.h>

/* !!!!!! ASSERTION: WHEN CONVERTING TO BYTE THE MAXIMUM VALUE IS 2^8 - 1 == 255 !!!!!! */

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