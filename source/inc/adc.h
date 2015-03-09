

#define ADC_PORT			MDR_PORTD
#define ADC_PIN_CURRENT		2
#define ADC_PIN_VOLTAGE		3
#define ADC_PIN_CONTRAST	4



void ADC_Initialize(void);
void ADC_UpdateLoopCurrent(void);
uint32_t ADC_GetLoopCurrent(void);
void ADC_UpdateLoopVoltage(void);
uint32_t ADC_GetLoopVoltage(void);

