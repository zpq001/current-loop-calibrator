
#include "linear_calibration.h"

#ifndef __DAC_H_
#define __DAC_H_

#define DAC_PORT        MDR_PORTE
#define DAC_OUTPUT_PIN  0
#define PROFILE_COUNT	2


enum DacModes {DAC_MODE_CONST, DAC_MODE_WAVEFORM};
enum SignalWaveforms {WAVE_MEANDR, WAVE_SAW_DIRECT, WAVE_SAW_REVERSED};


void DAC_Initialize(void);
void DAC_UpdateOutput(uint32_t value);	// Directly sets output to value
										// Used only for calibration

uint32_t DAC_GetCalibrationPoint(uint8_t pointNumber);
void DAC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue);
void DAC_Calibrate(void);
void DAC_ApplyCalibration(void);
void DAC_SaveCalibration(void);
void DAC_RestoreSettings(void);
void DAC_SaveSettings(void);

void DAC_SetSettingConst(uint32_t newValue);
void DAC_SetProfile(int16_t num);
void DAC_SetSettingWaveMax(uint32_t value);
void DAC_SetSettingWaveMin(uint32_t value);
void DAC_SetWaveform(uint8_t newWaveForm);
void DAC_SetPeriod(uint32_t new_period);
void DAC_SetMode(uint8_t new_mode);
void DAC_SetTotalCycles(uint32_t number);
void DAC_RestartCycles(void);
		
uint32_t DAC_GetSettingConst(void);
uint8_t DAC_GetActiveProfile(void);
uint32_t DAC_GetSettingWaveMax(void);
uint32_t DAC_GetSettingWaveMin(void);
uint8_t DAC_GetWaveform(void);
uint16_t DAC_GetPeriod(void);
uint8_t DAC_GetMode(void);
uint32_t DAC_GetTotalCycles(void);
uint32_t DAC_GetCurrentCycle(void);


#endif

