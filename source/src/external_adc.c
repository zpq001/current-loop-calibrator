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

#define CH_VGND			0
#define CH_LOW_GAIN		1
#define CH_HIGH_GAIN	2

static calibration_t adc_calibration_low_gain;
static calibration_t adc_calibration_high_gain;

static int32_t ext_current;
static uint8_t ext_current_range;

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
	adc_calibration_low_gain.point1.code = 2048;
	adc_calibration_low_gain.point2.value = 400000;
	adc_calibration_low_gain.point2.code = 2048+1886;
    adc_calibration_low_gain.scale = 10000L;
	CalculateCoefficients(&adc_calibration_low_gain);
    
    adc_calibration_high_gain.point1.value = 0;
	adc_calibration_high_gain.point1.code = 2048;
	adc_calibration_high_gain.point2.value = 40000;
	adc_calibration_high_gain.point2.code = 2048+1886;
    adc_calibration_high_gain.scale = 10000L;
	CalculateCoefficients(&adc_calibration_high_gain);
    
}



uint16_t getData(uint8_t channel) {
	uint16_t temp16u;
	channel &= 0x07;
	channel |= (1<<3);	// single-ended
	channel |= (1<<4);	// start
	SSP_SendData(MDR_SSP1, channel);
	while( SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE)!= SET );
	temp16u = SSP_ReceiveData(MDR_SSP1);
	return temp16u;
}



void ExtADC_UpdateCurrent(void) {
    volatile uint16_t conversion_result[3];
    conversion_result[0] = getData(CH_VGND);
	DWT_DelayUs(100);
    conversion_result[1] = getData(CH_LOW_GAIN);
	DWT_DelayUs(100);
    conversion_result[2] = getData(CH_HIGH_GAIN);
    // Pseudo-differential
    conversion_result[1] += 2048;
    conversion_result[2] += 2048;
    conversion_result[1] -= conversion_result[0];
    conversion_result[2] -= conversion_result[0];
    
    if ((conversion_result[2] > 100) && (conversion_result[2] < 4000)) {
        ext_current = GetValueForCode(&adc_calibration_high_gain, conversion_result[2]); 
        ext_current_range = EXTADC_LOW_RANGE;
    } else {
        ext_current = GetValueForCode(&adc_calibration_low_gain, conversion_result[1]);
        if ((conversion_result[1] > 100) && (conversion_result[1] < 4000))
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



