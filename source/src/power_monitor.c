/*******************************************************************
	Module power_monitor
	
	Power supply monitor

********************************************************************/
#include <string.h>
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_comp.h"
#include "hw_utils.h"
#include "eeprom.h"
#include "dac.h"
#include "power_monitor.h"


uint8_t device_mode;

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
    COMP_CVRefInitStructure.COMP_CVRefScale = COMP_CVREF_SCALE_14_div_32;
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
	
	NVIC_EnableIRQ(COMPARATOR_IRQn);
}


void COMPARATOR_IRQHandler(void) {
    // Save all settings to EEPROM here
	// When we get here,  main power supply is off and MCU operates from on-board capacitors
	// Disable all power-consuming devices and reduce F_CPU
    __disable_irq();
	// Stop watchdog
	// TODO
	
	PORT_DeInit(MDR_PORTA);
	PORT_DeInit(MDR_PORTB);
	PORT_DeInit(MDR_PORTC);
	PORT_DeInit(MDR_PORTD);
	PORT_DeInit(MDR_PORTE);
	PORT_DeInit(MDR_PORTF);
	
	hw_Switch_CPU_Clock_to_HSI();
	
	if (device_mode == MODE_NORMAL) {
		// Gather settings and states from all modules (using global variable settings)
		DAC_SaveSettings();
		// Add more modules if required
		
		EE_SaveSettings();
	} else {
		// System settings are already updated
		EE_SaveSystemSettings();
	}
	
	// Safely wait until power suply goes down.
	while(1);
}

