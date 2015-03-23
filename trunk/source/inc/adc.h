

#define ADC_PORT			MDR_PORTD
#define ADC_PIN_CURRENT		2
#define ADC_PIN_VOLTAGE		3
#define ADC_PIN_CONTRAST	4

#define LOOP_BREAK_TRESHOLD 3900    // [uA]
#define LOOP_ERROR_TRESHOLD 100     // [uA]
// Loop errors may be ORed
#define LOOP_OK     0               // Loop current is equal to the setting
#define LOOP_BREAK  0x1             // Loop current is less than 4mA
#define LOOP_ERROR  0x2             // Loop current is not equal to the setting


void ADC_Initialize(void);

void ADC_UpdateLoopCurrent(void);
void ADC_UpdateLoopMonitor(void);
uint8_t ADC_GetLoopStatus(void);
uint32_t ADC_GetLoopCurrent(void);
void ADC_SaveLoopCurrentCalibrationPoint(uint8_t pointNum, uint32_t measuredValue);
void ADC_LoopCurrentCalibrate(void);
void ADC_LC_ApplyCalibration(void);
void ADC_LC_SaveCalibration(void);

void ADC_UpdateLoopVoltage(void);
uint32_t ADC_GetLoopVoltage(void);
void ADC_SaveLoopVoltageCalibrationPoint(uint8_t pointNum, uint32_t measuredValue);
void ADC_LoopVoltageCalibrate(void);
void ADC_LV_ApplyCalibration(void);
void ADC_LV_SaveCalibration(void);

