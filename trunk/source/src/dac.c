/*******************************************************************
	Module dac.c
	
		Low-level functions for DAC

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_dac.h"
#include "MDR32F9Qx_port.h"
#include "dac.h"


void DAC_Initialize(void) {
    PORT_InitTypeDef PORT_InitStructure;
    
    // Setup GPIO
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << DAC_OUTPUT_PIN);
	PORT_Init(DAC_PORT, &PORT_InitStructure);
    
    // Setup DAC
    DAC2_Init(DAC2_REF);
    DAC2_Cmd(ENABLE);
    DAC2_SetData(0);   
}



