
#include "linear_calibration.h"


#define DAC_PORT        MDR_PORTE
#define DAC_OUTPUT_PIN  0

enum SignalWaveforms {WAVE_MEANDR, WAVE_SAW_DIRECT, WAVE_SAW_REVERSED};



void DAC_Initialize(void);
void DAC_SetCurrent(uint32_t new_value);
void DAC_ToggleEnabled(void);
uint32_t DAC_GetCalibrationPoint(uint8_t pointNumber);
void DAC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue);
void DAC_Calibrate(void);
void DAC_ApplyCalibration(calibration_t *points);
void DAC_SaveCalibration(calibration_t *points);

uint32_t DAC_GetSettingConst(void);
uint32_t DAC_GetSettingAlternHigh(void);
uint32_t DAC_GetSettingAlternLow(void);
void DAC_SetSettingConst(uint32_t newValue);


uint8_t DAC_GetWaveform(void);
void DAC_SetWaveform(uint8_t newWaveForm);

uint16_t DAC_GetPeriod(void);
void DAC_SetPeriod(uint16_t newPeriod);

void DAC_SetWaveformMode(void);
void DAC_SetConstantMode(void);

