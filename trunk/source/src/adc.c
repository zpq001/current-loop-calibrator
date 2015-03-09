

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_port.h"
#include "adc.h"


uint32_t adc_loop_voltage;
uint32_t adc_loop_current;
uint32_t adc_temperature;


void ADC_Initialize(void) {
	
	ADC_InitTypeDef sADC;
	ADCx_InitTypeDef sADCx;
	PORT_InitTypeDef PORT_InitStructure;
	
	ADC_StructInit(&sADC);
	sADC.ADC_TempSensor           = ADC_TEMP_SENSOR_Enable;
	sADC.ADC_TempSensorAmplifier  = ADC_TEMP_SENSOR_AMPLIFIER_Enable;
	sADC.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;
	ADC_Init (&sADC);
	
	// ADC1 Configuration 
	ADCx_StructInit (&sADCx);
	sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_ADC;
	//sADCx.ADC_ChannelNumber    = ADC_CH_TEMP_SENSOR;		
	sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_EXTERNAL;
	sADCx.ADC_Prescaler        = ADC_CLK_div_128;
	sADCx.ADC_DelayGo          = 7;
	ADC1_Init (&sADCx);
	ADC2_Init (&sADCx);
	
	// Disable ADC interupts
	//ADC1_ITConfig((ADCx_IT_END_OF_CONVERSION  | ADCx_IT_OUT_OF_RANGE), DISABLE);
	//ADC2_ITConfig((ADCx_IT_END_OF_CONVERSION  | ADCx_IT_OUT_OF_RANGE), DISABLE);
	
	// ADC1 enable
	ADC1_Cmd (ENABLE);
	ADC2_Cmd (ENABLE);
	
	// Setup GPIO
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = ((1 << ADC_PIN_CURRENT) | (1 << ADC_PIN_VOLTAGE) | (1 << ADC_PIN_CONTRAST));
	PORT_Init(ADC_PORT, &PORT_InitStructure);
}

// Using ADC1
void ADC_UpdateLoopCurrent(void) {
	ADC1_SetChannel(ADC_PIN_CURRENT);
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADCx_FLAG_END_OF_CONVERSION) == RESET);
	adc_loop_current = ADC1_GetResult();
}

uint32_t ADC_GetLoopCurrent(void) {
	return adc_loop_current;	// FIXME
}

// Using ADC1
void ADC_UpdateLoopVoltage(void) {
	ADC1_SetChannel(ADC_PIN_VOLTAGE);
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADCx_FLAG_END_OF_CONVERSION) == RESET);
	adc_loop_voltage = ADC1_GetResult();
}

uint32_t ADC_GetLoopVoltage(void) {
	return adc_loop_voltage;	// FIXME
}

// Using ADC1
void ADC_UpdateMCUTemperature(void) {
	ADC1_SetChannel(ADC_CH_TEMP_SENSOR);
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADCx_FLAG_END_OF_CONVERSION) == RESET);
	adc_temperature = ADC1_GetResult();
}



// Using ADC2
void ADC_UpdateContrastVoltage(void) {
	ADC2_SetChannel(ADC_PIN_CONTRAST);
}







