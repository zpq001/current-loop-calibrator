/*******************************************************************
	Module power_monitor
	
	Power supply monitor

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_comp.h"
#include "power_monitor.h"


void PowerMonitor_Init(void) {
    
    COMP_InitTypeDef COMP_InitStructure;
    COMP_CVRefInitTypeDef COMP_CVRefInitStructure;
    PORT_InitTypeDef PORT_InitStructure;
    
    COMP_StructInit(&COMP_InitStructure);
    COMP_InitStructure.COMP_PlusInputSource = COMP_PlusInput_CVREF;  
    COMP_InitStructure.COMP_MinusInputSource = COMP_MinusInput_IN1;
    COMP_Init(&COMP_InitStructure);
    
    COMP_CVRefStructInit(&COMP_CVRefInitStructure);
    COMP_CVRefInitStructure.COMP_CVRefSource = COMP_CVREF_SOURCE_AVdd;
    COMP_CVRefInitStructure.COMP_CVRefRange = COMP_CVREF_RANGE_Up;
    COMP_CVRefInitStructure.COMP_CVRefScale = COMP_CVREF_SCALE_9_div_32;
    COMP_CVRefInit(&COMP_CVRefInitStructure);
    
    COMP_Cmd(ENABLE); 
    COMP_CVRefCmd(ENABLE);    
    while(COMP_GetCfgFlagStatus(COMP_CFG_FLAG_READY) != SET);
    
    // Setup GPIO
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << POWER_MONITOR_PIN);
	PORT_Init(POWER_MONITOR_PORT, &PORT_InitStructure);
    
    // Read and clear comparator result latch
    COMP_GetResultLatch();
    COMP_ITConfig(ENABLE);
}


void COMPARATOR_IRQHandler(void) {
    // Save all settings to EEPROM here
    // TODO
    
}

