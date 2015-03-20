#ifndef __LINEAR_CALIBRATION_H
#define __LINEAR_CALIBRATION_H


#ifdef __cplusplus
extern "C" {
#endif



#include <stdint.h>


typedef struct {
	int32_t value;
	uint32_t code;
} calibration_point_t;

typedef struct {
	int32_t k;
	int32_t offset;
} line_coeff_t;

typedef struct {
    calibration_point_t point1;
    calibration_point_t point2;
    int32_t scale;
    line_coeff_t coeff;
} calibration_t;

int32_t DivI32Rnd(int32_t value, int32_t divider);
void CalculateCoefficients(calibration_t *c);
uint32_t GetCodeForValue(calibration_t *c, int32_t value);
int32_t GetValueForCode(calibration_t *c, uint32_t code);

void CreateSawWaveform(uint16_t *buffer, int16_t startValue, int16_t stopValue, uint16_t size);

#endif




#ifdef __cplusplus
}
#endif
