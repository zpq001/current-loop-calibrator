
#define DAC_PORT        MDR_PORTE
#define DAC_OUTPUT_PIN  0


typedef struct {
	uint32_t value;
	uint32_t dac_code;
} dac_calibration_point_t;

typedef struct {
	dac_calibration_point_t point1;
	dac_calibration_point_t point2;
} dac_calibration_t;



void DAC_Initialize(void);
void DAC_SetCurrent(uint32_t new_value);
void DAC_ToggleEnabled(void);
uint32_t DAC_GetCalibrationPoint(uint8_t pointNumber);
void DAC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue);
void DAC_Calibrate(void);
void DAC_ApplyCalibration(dac_calibration_t *points);
void DAC_SaveCalibration(dac_calibration_t *points);





