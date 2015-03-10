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
#include "linear_calibration.h"
#include "dac.h"
#include "led.h"

#define DEFAULT_OFF_VALUE		4000


static struct {
	uint32_t setting;
	uint32_t dac_code;
	uint8_t isEnabled;
} dac_state;

static calibration_t dac_calibration;


static void DAC_UpdateState(void) {
	uint32_t temp32u;
	if (dac_state.isEnabled) {
		temp32u = GetCodeForValue(&dac_calibration, dac_state.setting);
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
	
	// Default calibration
	dac_calibration.point1.value = 4000;
	dac_calibration.point1.code = 655;
	dac_calibration.point2.value = 20000;
	dac_calibration.point2.code = 3276;
    dac_calibration.scale = 10000L;
	CalculateCoefficients(&dac_calibration);
	
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
	calibration_point_t *p = (pointNum == 1) ? &dac_calibration.point1 : &dac_calibration.point2;
	p->value = measuredValue;
	p->code = dac_state.dac_code;
}


void DAC_Calibrate(void) {
	CalculateCoefficients(&dac_calibration);
	DAC_UpdateState();
}


void DAC_ApplyCalibration(calibration_t *c) {           // FIXME
	dac_calibration.point1.value = c->point1.value;
	dac_calibration.point1.code = c->point1.code;
	dac_calibration.point2.value = c->point2.value;
	dac_calibration.point2.code = c->point2.code;
	CalculateCoefficients(&dac_calibration);
	DAC_UpdateState();
}


void DAC_SaveCalibration(calibration_t *points) {           // FIXME
	points->point1.value = dac_calibration.point1.value;
	points->point1.code = dac_calibration.point1.code;
	points->point2.value = dac_calibration.point2.value;
	points->point2.code = dac_calibration.point2.code;
}



