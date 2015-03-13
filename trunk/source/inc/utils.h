#ifndef __UTILS_H_
#define __UTILS_H_

#define NO_TERMINATING_ZERO	0x80
#define SPACE_CHAR    ' '
#define DOT_CHAR      '.'
#define MINUS_CHAR    '-'
#define ZERO_CHAR     '0'

#include <stdint.h>

uint8_t i32toa_align_right(int32_t val, char *buffer, uint8_t len, uint8_t min_digits_required, int8_t dot_position);




#endif
