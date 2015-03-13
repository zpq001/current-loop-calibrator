

#define ADC_PORT			MDR_PORTD
#define ADC_PIN_CURRENT		2
#define ADC_PIN_VOLTAGE		3
#define ADC_PIN_CONTRAST	4

#define LOOP_BREAK_TRESHOLD 3900    // [uA]
#define LOOP_OK     0
#define LOOP_BREAK  1


void ADC_Initialize(void);

void ADC_UpdateLoopCurrent(void);
void ADC_UpdateLoopMonitor(void);
uint32_t ADC_GetLoopCurrent(void);
uint8_t ADC_GetLoopStatus(void);

void ADC_UpdateLoopVoltage(void);
uint32_t ADC_GetLoopVoltage(void);

