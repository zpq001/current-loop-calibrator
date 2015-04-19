
#ifndef __EEPROM_H_
#define __EEPROM_H_

#include "dac.h"
#include "linear_calibration.h"



typedef struct {
	struct {
		uint32_t setting[DAC_PROFILE_COUNT];		// [uA]
		uint8_t profile;
		uint8_t waveform;
		uint32_t period;
		uint32_t wave_min;
		uint32_t wave_max;
		uint32_t total_cycles;
	} dac;
	uint32_t num_power_cycles;
	uint16_t crc;
	uint32_t dummy;     // Padding for 32-bit word access
} settings_t;

typedef struct {
	struct {
		calibration_point_t point1;
		calibration_point_t point2;
	} dac;
	struct {
		calibration_point_t point1;
		calibration_point_t point2;
	} adc_voltage;
	struct {
		calibration_point_t point1;
		calibration_point_t point2;
	} adc_current;
	struct {
		calibration_point_t point1;
		calibration_point_t point2;
	} extadc_high_gain;
	struct {
		calibration_point_t point1;
		calibration_point_t point2;
	} extadc_low_gain;
	uint8_t contrast_level;
	uint8_t sound_state;
	uint16_t crc;
	uint32_t dummy;     // Padding for 32-bit word access
} system_settings_t;



extern settings_t settings;
extern system_settings_t system_settings;

uint8_t EE_RestoreSettings(void);
uint8_t EE_RestoreSystemSettings(uint8_t erase_current);
uint32_t EE_GetPowerCyclesCount(void);
void EE_SaveSettings(void);
void EE_SaveSystemSettings(void);


#endif

