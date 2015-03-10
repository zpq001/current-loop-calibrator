#ifndef __LINEAR_CALIBRATION_H
#define __LINEAR_CALIBRATION_H

#include <stdint.h>


typedef struct {
	int32_t value;
	uint32_t code;
} calibration_point_t;

typedef struct {
	uint32_t k;
	uint32_t offset;
} line_coeff_t;

typedef struct {
    calibration_point_t point1;
    calibration_point_t point2;
    uint32_t scale;
    line_coeff_t coeff;
} calibration_t;


void CalculateCoefficients(calibration_t *c);
uint32_t GetCodeForValue(calibration_t *c, int32_t value);
int32_t GetValueForCode(calibration_t *c, uint32_t code);

#endif
