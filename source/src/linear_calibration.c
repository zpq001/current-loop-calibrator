
#include "linear_calibration.h"


void CalculateCoefficients(calibration_t *c) {
    int32_t temp = c->point2.code - c->point1.code;
	c->coeff.k = ((int32_t)(c->point2.value - c->point1.value) * c->scale + (int32_t)(temp>>1)) / ((int32_t)temp);	// Round	
	c->coeff.offset = (int32_t)c->point1.value * c->scale - (int32_t)c->point1.code * c->coeff.k;
}

uint32_t GetCodeForValue(calibration_t *c, int32_t value) {
    return (uint32_t)((value * c->scale - c->coeff.offset + (c->coeff.k>>1)) / c->coeff.k);	// Round
}

int32_t GetValueForCode(calibration_t *c, uint32_t code) {
    return (int32_t)(((int32_t)code * c->coeff.k + c->coeff.offset + (c->scale>>1)) / (c->scale));	// Round
}






