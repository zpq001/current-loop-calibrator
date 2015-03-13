

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_port.h"
#include "linear_calibration.h"
#include "led.h"
#include "adc.h"


static uint32_t loop_voltage;
static uint32_t loop_current;
static uint8_t loop_status;
static uint32_t temperature;


static calibration_t adc_voltage_calibration;
static calibration_t adc_current_calibration;

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
    
   	// Default calibration
	adc_voltage_calibration.point1.value = 0;
	adc_voltage_calibration.point1.code = 0;
	adc_voltage_calibration.point2.value = 20000;
	adc_voltage_calibration.point2.code = 3276;
    adc_voltage_calibration.scale = 10000L;
	CalculateCoefficients(&adc_voltage_calibration);
    
    adc_current_calibration.point1.value = 0;
	adc_current_calibration.point1.code = 0;
	adc_current_calibration.point2.value = 2000;
	adc_current_calibration.point2.code = 3276;
    adc_current_calibration.scale = 10000L;
	CalculateCoefficients(&adc_current_calibration);
}

// Using ADC1
void ADC_UpdateLoopCurrent(void) {
    uint32_t temp32u;
	ADC1_SetChannel(ADC_PIN_CURRENT);
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADCx_FLAG_END_OF_CONVERSION) == RESET);
	temp32u = ADC1_GetResult();
    loop_current = GetValueForCode(&adc_current_calibration, temp32u);        
}

void ADC_UpdateLoopMonitor(void) {
    if (loop_current <= LOOP_BREAK_TRESHOLD) {
        loop_status = LOOP_BREAK;
		LED_Set(LED_BREAK, 1);
    } else {
        loop_status = LOOP_OK;
		LED_Set(LED_BREAK, 0);
    }
}

uint8_t ADC_GetLoopStatus(void) {
    return loop_status;
}

uint32_t ADC_GetLoopCurrent(void) {
	return loop_current;
}

// Using ADC1
void ADC_UpdateLoopVoltage(void) {
	uint32_t temp32u;
    ADC1_SetChannel(ADC_PIN_VOLTAGE);
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADCx_FLAG_END_OF_CONVERSION) == RESET);
	temp32u = ADC1_GetResult();
    loop_voltage = GetValueForCode(&adc_voltage_calibration, temp32u); 
}

uint32_t ADC_GetLoopVoltage(void) {
	return loop_voltage;
}

// Using ADC1
void ADC_UpdateMCUTemperature(void) {
	ADC1_SetChannel(ADC_CH_TEMP_SENSOR);
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADCx_FLAG_END_OF_CONVERSION) == RESET);
	temperature = ADC1_GetResult();     // FIXME
}


// Using ADC2
void ADC_UpdateContrastVoltage(void) {
	ADC2_SetChannel(ADC_PIN_CONTRAST);
}







