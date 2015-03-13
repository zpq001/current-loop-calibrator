
#include <stdint.h>
#include "utils.h"

//-------------------------------------------------------//
//	Converts 32-bit integer to a right - aligned string
//	input:
//		val - 32-bit value to convert, signed
//		*buffer - pointer to store result data
//		len[6:0] - output string length, including \0 symbol
//				If result length < len[6:0], result will be cut to fit buffer
//				If result length > len[6:0], buffer will be filled with spaces
//		len[7] - if not set, result string will be null-terminated
//      dot_position - a dot symbol will appear after char with this index (starting from rightmost, whose index is 0)
//          set negative if not required
//  output:
//      returns number of chars (except leading spaces)
//-------------------------------------------------------//
uint8_t i32toa_align_right(int32_t val, char *buffer, uint8_t len, uint8_t min_digits_required, int8_t dot_position)
{
    uint8_t is_negative = 0;
    uint8_t index = 0;
    if (!len)	return 0;

    if (val < 0) {
        val = -val;
        is_negative = 1;
    }

    // Fill from last element
    buffer += len - 1;

    if (len & NO_TERMINATING_ZERO) {
        len = len & ~NO_TERMINATING_ZERO;    
    } else {
        *buffer-- = 0;
        len--;
    }
    if (!len)	return 0;

    do {
        if (index == dot_position) {
            *buffer-- = DOT_CHAR;
        } else {
            *buffer-- = val % 10 + ZERO_CHAR;
            val /= 10;
            if (min_digits_required)
                min_digits_required--;
        }
        index++;
        len--;
    }
    while ((val != 0) && len);

    // Pad with 0s
    while ((min_digits_required) && (len)) {
        if (index == dot_position) {
            *buffer-- = DOT_CHAR;
        } else {
            *buffer-- = ZERO_CHAR;
            if (min_digits_required)
                min_digits_required--;
        }
        index++;
        len--;
    }

    // Minus sign
    if ((len) && (is_negative)) {
        *buffer-- = MINUS_CHAR;
        index++;
        len--;
    }

    // Padding with spaces
    while(len--)
        *buffer-- = SPACE_CHAR;

    return index;
}



