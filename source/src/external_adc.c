/*******************************************************************
	Module aexternal_adc
	
	External ADC interface

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_ssp.h"
#include "external_adc.h"


#define CH_VGND			0
#define CH_LOW_GAIN		1
#define CH_HIGH_GAIN	2



void ExtADC_Initialize(void) {
	
	SSP_InitTypeDef sSSP;
	PORT_InitTypeDef PORT_InitStructure;
	
	// SPI frequency = F_SSPCLK / ( CPSDVR * (1 + SCR) ) 
	SSP_BRGInit(MDR_SSP2,SSP_HCLKdiv1);		// F_SSPCLK = HCLK / 1
	
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
}



uint16_t getData(uint8_t channel) {
	uint16_t temp16u;
	channel &= ~0x07;
	channel |= (1<<3);	// single-ended
	channel |= (1<<4);	// start
	SSP_SendData(MDR_SSP1, channel);
	temp16u = SSP_ReceiveData(MDR_SSP1);
	return temp16u;
}





