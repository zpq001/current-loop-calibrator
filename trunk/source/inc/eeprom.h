
#ifndef __EEPROM_H_
#define __EEPROM_H_


#include "linear_calibration.h"



typedef struct {
	struct {
		calibration_point_t calibration_point1;
		calibration_point_t calibration_point2;
		uint32_t setting;		// [uA]
		uint8_t waveform;
		uint32_t period;
		uint32_t wave_min;
		uint32_t wave_max;
		uint32_t total_cycles;
	} dac;
	uint16_t crc;
	uint32_t dummy;
} settings_t;



extern settings_t settings;

uint8_t EE_RestoreSettings(void);
void EE_SaveSettings(void);


#endif

