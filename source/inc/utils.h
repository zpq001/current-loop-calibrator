#ifndef __UTILS_H_
#define __UTILS_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

// iXXtoa stuff
#define NO_TERMINATING_ZERO	0x80
#define SPACE_CHAR    ' '
#define DOT_CHAR      '.'
#define MINUS_CHAR    '-'
#define ZERO_CHAR     '0'

// Return values for verify
#define VALUE_IN_RANGE          0
#define VALUE_BOUND_BY_MIN      (1<<0)
#define VALUE_BOUND_BY_MAX      (1<<1)


uint8_t i32toa_align_right(int32_t val, char *buffer, uint8_t len, uint8_t min_digits_required, int8_t dot_position);
uint8_t verify_uint32(uint32_t *value, uint32_t min, uint32_t max);
uint8_t verify_int32(int32_t *value, int32_t min, int32_t max);

void round_uint32(uint32_t *value, uint8_t digit);
void round_int32(int32_t *value, uint8_t digit);


#ifdef __cplusplus
}
#endif

#endif
