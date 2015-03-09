/*******************************************************************
	Module dac.c
	
		Low-level functions for DAC


Calibration sequence:

	DAC_Initialize();
	// Point 1
	temp32u = DAC_GetCalibrationPoint(1);
	DAC_SetCurrent(temp32);
	... user enters actual current value, saved in meaValue ...
	DAC_SaveCalibrationPoint(1, meaValue);
	ADC_SaveCalibrationPoint(1, meaValue);
	// Point 2
	temp32u = DAC_GetCalibrationPoint(1);
	DAC_SetCurrent(temp32);
	... user enters actual current value, saved in meaValue ...
	DAC_SaveCalibrationPoint(2, meaValue);
	ADC_SaveCalibrationPoint(2, meaValue);
	// Done!
	DAC_Calibrate();

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_dac.h"
#include "MDR32F9Qx_port.h"
#include "dac.h"
#include "led.h"

#define COEFF_SCALE				10000L
#define DEFAULT_OFF_VALUE		4000

static struct {
	uint32_t k;
	uint32_t offset;
} line_coeffs;

static struct {
	uint32_t setting;
	uint32_t dac_code;
	uint8_t isEnabled;
} dac_state;

static dac_calibration_t dac_calibration;

static void DAC_CalculateCoeffs(void) {
	int32_t temp = dac_calibration.point2.dac_code - dac_calibration.point1.dac_code;
	line_coeffs.k = ((int32_t)(dac_calibration.point2.value - dac_calibration.point1.value) * COEFF_SCALE + (int32_t)(temp>>1)) / ((int32_t)temp);	// Round	
	line_coeffs.offset = (int32_t)dac_calibration.point1.value * COEFF_SCALE - (int32_t)dac_calibration.point1.dac_code * line_coeffs.k;
}

static uint32_t DAC_GetCodeForValue(uint32_t value) {
	return (uint32_t)(((int32_t)value * COEFF_SCALE - line_coeffs.offset + (line_coeffs.k>>1)) / line_coeffs.k);	// Round
}

static void DAC_UpdateState(void) {
	uint32_t temp32u;
	if (dac_state.isEnabled) {
		temp32u = DAC_GetCodeForValue(dac_state.setting);
		if (temp32u > 4095)
			temp32u = 4095;
		DAC2_SetData(temp32u);
		dac_state.dac_code = temp32u;
		LED_Set(LED_STATE, 1);
	} else {
		DAC2_SetData(DEFAULT_OFF_VALUE);
		dac_state.dac_code = DEFAULT_OFF_VALUE;
		LED_Set(LED_STATE, 0);
	}
}


void DAC_Initialize(void) {
    PORT_InitTypeDef PORT_InitStructure;
    
    // Setup GPIO
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << DAC_OUTPUT_PIN);
	PORT_Init(DAC_PORT, &PORT_InitStructure);
	
	// Setup DAC
    DAC2_Init(DAC2_REF);
    DAC2_Cmd(ENABLE);
    DAC2_SetData(0);  
	
	// Ideal calibration
	dac_calibration.point1.value = 4000;
	dac_calibration.point1.dac_code = 655;
	dac_calibration.point2.value = 20000;
	dac_calibration.point2.dac_code = 3276;
	DAC_CalculateCoeffs();
	
	// Default state after power-on
	dac_state.isEnabled = 0;
	dac_state.setting = 0;
	DAC2_SetData(0);
	LED_Set(LED_STATE, 0);
}


// Sets DAC output to specified value [uA]
void DAC_SetCurrent(uint32_t new_value) {
	dac_state.setting = new_value;
	DAC_UpdateState();
}


// Enables or disables DAC output
void DAC_ToggleEnabled(void) {
	dac_state.isEnabled = !dac_state.isEnabled;
	DAC_UpdateState();
}


uint32_t DAC_GetCalibrationPoint(uint8_t pointNumber) {
	uint32_t temp32u;
	if (pointNumber == 1)
		temp32u = 4000;
	else
		temp32u = 20000;
	return temp32u;
}


void DAC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue) {
	dac_calibration_point_t *p = (pointNum == 1) ? &dac_calibration.point1 : &dac_calibration.point2;
	p->value = measuredValue;
	p->dac_code = dac_state.dac_code;
}


void DAC_Calibrate(void) {
	DAC_CalculateCoeffs();
	DAC_UpdateState();
}


void DAC_ApplyCalibration(dac_calibration_t *points) {
	dac_calibration.point1.value = points->point1.value;
	dac_calibration.point1.dac_code = points->point1.dac_code;
	dac_calibration.point2.value = points->point2.value;
	dac_calibration.point2.dac_code = points->point2.dac_code;
	DAC_CalculateCoeffs();
	DAC_UpdateState();
}


void DAC_SaveCalibration(dac_calibration_t *points) {
	points->point1.value = dac_calibration.point1.value;
	points->point1.dac_code = dac_calibration.point1.dac_code;
	points->point2.value = dac_calibration.point2.value;
	points->point2.dac_code = dac_calibration.point2.dac_code;
}



