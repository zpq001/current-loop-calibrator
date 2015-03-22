/****************************************************************//*
	@brief Module led
	
	Functions for LEDs
    
    
********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "led.h"


void LED_Init(void) {
    PORT_InitTypeDef PORT_InitStructure;
    // Setup GPIO
    PORT_ResetBits(LED_PORT, (1<<LED_ERROR) | (1<<LED_BREAK));
    
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << LED_ERROR) | (1 << LED_BREAK);
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_OE = PORT_OE_OUT;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(LED_PORT, &PORT_InitStructure);
}

void LED_Set(uint8_t led, uint8_t state) {
    if ((led == LED_ERROR) || (led == LED_BREAK)) {
        if (state)
            PORT_SetBits(LED_PORT, (1<<led));
        else
            PORT_ResetBits(LED_PORT, (1<<led));
    }
}

