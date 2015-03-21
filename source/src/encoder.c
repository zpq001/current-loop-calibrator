/*******************************************************************
	Module encoder.c
	
		Low-level functions for incremental encoder.

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "encoder.h"

int16_t encoder_delta;
static volatile int16_t encoder_counter;

void Encoder_Init(void) {
    PORT_InitTypeDef PORT_InitStructure;
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << ENCODER_PINA) | (1 << ENCODER_PINB);
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_OE = PORT_OE_IN;
    PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(ENCODER_PORT, &PORT_InitStructure);
    
    encoder_counter = 0;
}

void Encoder_Process(void) {
	static uint8_t enc_state = 0;
	enc_state = enc_state << 2;
	
	// Get new encoder state
	if (ENCODER_PORT->RXTX & (1<<ENCODER_PINA)) 
		enc_state |= 0x01;
	if (ENCODER_PORT->RXTX & (1<<ENCODER_PINB)) 
		enc_state |= 0x02;
	
	// Detect direction
	switch(enc_state & 0x0F) {
		case 0x2: case 0x4: case 0xB: case 0xD:
			encoder_counter --;
		break;
		case 0x1: case 0x7: case 0x8: case 0xE:
			encoder_counter ++;
		break;
	}
}

void Encoder_UpdateDelta(void) {
	static int16_t old_counter = 0;
	int16_t delta = old_counter;
	old_counter = encoder_counter&0xFFFC;
	delta = old_counter - delta;
	encoder_delta = delta>>2;
    encoder_delta *= (encoder_delta < 0) ? -encoder_delta : encoder_delta;
}


