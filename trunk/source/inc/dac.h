
#include "linear_calibration.h"


#define DAC_PORT        MDR_PORTE
#define DAC_OUTPUT_PIN  0





void DAC_Initialize(void);
void DAC_SetCurrent(uint32_t new_value);
void DAC_ToggleEnabled(void);
uint32_t DAC_GetCalibrationPoint(uint8_t pointNumber);
void DAC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue);
void DAC_Calibrate(void);
void DAC_ApplyCalibration(calibration_t *points);
void DAC_SaveCalibration(calibration_t *points);





