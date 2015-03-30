/****************************************************************//*
	@brief Module Linear calibration
	
	Contains math functions for linear sensor calibration
    Also contains optimized saw waveform generator using 
	Bresenham algorithm for drawing lines on pixel screens
    
********************************************************************/

#include "linear_calibration.h"


int32_t DivI32Rnd(int32_t value, int32_t divider) {
    value <<= 1;
    value /= divider;
    value += (value >= 0) ? 1 : -1;
    value /= 2;
    return value;
}

void CalculateCoefficients(calibration_t *c) {
    int32_t temp1 = c->point2.code - c->point1.code;
    int32_t temp2 = c->point2.value - c->point1.value;
    temp2 *= c->scale;
    c->coeff.k = DivI32Rnd(temp2, temp1);
    temp1 = c->point1.value * c->scale;
    temp2 = c->point1.code * c->coeff.k;
    c->coeff.offset = temp1 - temp2;
}

uint32_t GetCodeForValue(calibration_t *c, int32_t value) {
    int32_t temp = value * c->scale;
    temp -= c->coeff.offset;
    temp = DivI32Rnd(temp, c->coeff.k);
    return (uint32_t)temp;
}

int32_t GetValueForCode(calibration_t *c, uint32_t code) {
    int32_t temp = (int32_t)code * c->coeff.k;
    temp += c->coeff.offset;
    temp = DivI32Rnd(temp, c->scale);
    return temp;
}


void CreateSawWaveform(uint16_t *buffer, int16_t startValue, int16_t stopValue, uint16_t size) {

    int16_t dy = 0;
    int16_t dx = 0;
    int16_t stepx = 0;
    int16_t stepy = 0;
    int16_t fraction = 0;
    int16_t x1 = 0;
    int16_t x2 = size-1;

    dy = stopValue - startValue;
    dx = x2 - x1;
    if (dy < 0)
    {
        dy = -dy;
        stepy = -1;
    }
    else stepy = 1;
    if (dx < 0)
    {
        dx = -dx;
        stepx = -1;
    }
    else stepx = 1;
    dy <<= 1;
    dx <<= 1;
    buffer[x1] = startValue;
    if (dx > dy)
    {
        fraction = dy - (dx >> 1);
        while (x1 != x2)
        {
            if (fraction >= 0)
            {
                startValue += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;
            buffer[x1] = startValue;
        }
    }
    else
    {
        fraction = dx - (dy >> 1);
        while (startValue != stopValue)
        {
            if (fraction >= 0)
            {
                x1 += stepx;
                fraction -= dy;
            }
            startValue += stepy;
            fraction += dx;
            buffer[x1] = startValue;
        }
    }


}





