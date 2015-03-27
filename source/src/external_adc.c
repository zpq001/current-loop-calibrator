/*******************************************************************
	Module aexternal_adc
	
	External ADC interface

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_ssp.h"
#include "external_adc.h"
#include "linear_calibration.h"
#include "dwt_delay.h"
#include "eeprom.h"

#define CH_VGND			0
#define CH_LOW_GAIN		1
#define CH_HIGH_GAIN	2

#define EXTADC_OVERSAMPLE  4

static calibration_t adc_calibration_low_gain;
static calibration_t adc_calibration_high_gain;

static int32_t ext_current;
static uint8_t ext_current_range;

static uint32_t adc_low_gain_code;
static uint32_t adc_high_gain_code;

void ExtADC_Initialize(void) {
	
	SSP_InitTypeDef sSSP;
	PORT_InitTypeDef PORT_InitStructure;
	
	// SPI frequency = F_SSPCLK / ( CPSDVR * (1 + SCR) ) 
	SSP_BRGInit(MDR_SSP1,SSP_HCLKdiv1);		// F_SSPCLK = HCLK / 1
	
	SSP_StructInit (&sSSP);
	sSSP.SSP_SCR  = 31;			// 0 to 255
	sSSP.SSP_CPSDVSR = 10;		// even 2 to 254
	sSSP.SSP_Mode = SSP_ModeMaster;
	sSSP.SSP_WordLength = SSP_WordLength13b;
	sSSP.SSP_FRF = SSP_FRF_Microwire;
	sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
	
	SSP_Init (MDR_SSP1,&sSSP);
	SSP_Cmd(MDR_SSP1, ENABLE);
	
	
    // Setup GPIO
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << EXTADC_CLK_PIN) | (1 << EXTADC_TXD_PIN) | (1 << EXTADC_FSS_PIN);
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_Init(EXTADC_PORT, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin = (1 << EXTADC_RXD_PIN);
	PORT_InitStructure.PORT_OE    = PORT_OE_IN;
	PORT_Init(EXTADC_PORT, &PORT_InitStructure);
    
    
    // Default calibration
	adc_calibration_low_gain.point1.value = 0;
	adc_calibration_low_gain.point1.code = 4096 * EXTADC_OVERSAMPLE;
	adc_calibration_low_gain.point2.value = 400000;
	adc_calibration_low_gain.point2.code = (4096 + 1886) * EXTADC_OVERSAMPLE;
    adc_calibration_low_gain.scale = 1000L;
	CalculateCoefficients(&adc_calibration_low_gain);
    
    adc_calibration_high_gain.point1.value = 0;
	adc_calibration_high_gain.point1.code = 4096 * EXTADC_OVERSAMPLE;
	adc_calibration_high_gain.point2.value = 40000;
	adc_calibration_high_gain.point2.code = (4096 + 1886) * EXTADC_OVERSAMPLE;
    adc_calibration_high_gain.scale = 10000L;
	CalculateCoefficients(&adc_calibration_high_gain);
    
}



static uint32_t getData(uint8_t channel) {
	uint32_t temp32u;
    uint8_t i;
    temp32u = 0;
	channel &= 0x07;
	channel |= (1<<3);	// single-ended
	channel |= (1<<4);	// start
    for (i=0; i<EXTADC_OVERSAMPLE; i++) {
        SSP_SendData(MDR_SSP1, channel);
        while( SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE)!= SET );
        temp32u += SSP_ReceiveData(MDR_SSP1);
    }
	return temp32u;
}



void ExtADC_UpdateCurrent(void) {
    volatile uint32_t conversion_result[3];
    conversion_result[0] = getData(CH_VGND);
	DWT_DelayUs(100);
    conversion_result[1] = getData(CH_LOW_GAIN);
	DWT_DelayUs(100);
    conversion_result[2] = getData(CH_HIGH_GAIN);
    // Pseudo-differential
	adc_low_gain_code = conversion_result[1] + (4096*EXTADC_OVERSAMPLE);
	adc_high_gain_code = conversion_result[2] + (4096*EXTADC_OVERSAMPLE);
    adc_low_gain_code -= conversion_result[0];
    adc_high_gain_code -= conversion_result[0];
    
    // TODO : use signed arithmetic (remove 4096 offset - unnecessary)
    
    if ((conversion_result[2] > 100*EXTADC_OVERSAMPLE) && (conversion_result[2] < 4000*EXTADC_OVERSAMPLE)) {
        ext_current = GetValueForCode(&adc_calibration_high_gain, adc_high_gain_code);
        ext_current_range = EXTADC_LOW_RANGE;
    } else {
        ext_current = GetValueForCode(&adc_calibration_low_gain, adc_low_gain_code);
        if ((conversion_result[1] > 100*EXTADC_OVERSAMPLE) && (conversion_result[1] < 4000*EXTADC_OVERSAMPLE))
            ext_current_range = EXTADC_HIGH_RANGE;
        else
            ext_current_range = EXTADC_HIGH_OVERLOAD;
    }
}

int32_t ExtADC_GetCurrent(void) {
    return ext_current;
}

uint8_t ExtADC_GetRange(void) {
    return ext_current_range;
}


void ExtADC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue) {
    if (pointNum == 1) {
        // Calibration at zero - common for both ranges
        adc_calibration_low_gain.point1.value = 0;
        adc_calibration_low_gain.point1.code = adc_low_gain_code;
        adc_calibration_high_gain.point1.value = 0;
        adc_calibration_high_gain.point1.code = adc_low_gain_code;
    } else if (pointNum == 2) {
        // Calibration at 30mA
        adc_calibration_high_gain.point2.value = measuredValue;
        adc_calibration_high_gain.point2.code = adc_high_gain_code;
    } else {
        // Calibration at 300mA
        adc_calibration_low_gain.point2.value = measuredValue;
        adc_calibration_low_gain.point2.code = adc_low_gain_code;
    }
}


void ExtADC_Calibrate(void) {
	CalculateCoefficients(&adc_calibration_low_gain);
	CalculateCoefficients(&adc_calibration_high_gain);
}


void ExtADC_ApplyCalibration(void) {
    adc_calibration_low_gain.point1.value  = system_settings.extadc_low_gain.point1.value;
	adc_calibration_low_gain.point1.code   = system_settings.extadc_low_gain.point1.code;
	adc_calibration_low_gain.point2.value  = system_settings.extadc_low_gain.point2.value;
	adc_calibration_low_gain.point2.code   = system_settings.extadc_low_gain.point2.code;
    adc_calibration_high_gain.point1.value = system_settings.extadc_high_gain.point1.value;
	adc_calibration_high_gain.point1.code  = system_settings.extadc_high_gain.point1.code;
	adc_calibration_high_gain.point2.value = system_settings.extadc_high_gain.point2.value;
	adc_calibration_high_gain.point2.code  = system_settings.extadc_high_gain.point2.code;
    CalculateCoefficients(&adc_calibration_low_gain);
	CalculateCoefficients(&adc_calibration_high_gain);
}


void ExtADC_SaveCalibration(void) {
    system_settings.extadc_low_gain.point1.value  = adc_calibration_low_gain.point1.value;
	system_settings.extadc_low_gain.point1.code   = adc_calibration_low_gain.point1.code;
	system_settings.extadc_low_gain.point2.value  = adc_calibration_low_gain.point2.value;
	system_settings.extadc_low_gain.point2.code   = adc_calibration_low_gain.point2.code;
    system_settings.extadc_high_gain.point1.value = adc_calibration_high_gain.point1.value;
	system_settings.extadc_high_gain.point1.code  = adc_calibration_high_gain.point1.code;
	system_settings.extadc_high_gain.point2.value = adc_calibration_high_gain.point2.value;
	system_settings.extadc_high_gain.point2.code  = adc_calibration_high_gain.point2.code;
}









