/*******************************************************************
	Module systick
	
		Low-level system syncronizer
	Also contains fuinctions for LCD contrast PWM control

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_timer.h"
#include <string.h>
#include "systick.h"
#include "adc.h"
#include "lcd_contrast.h"
#include "encoder.h"
#include "sound.h"

SoftTimer16b_t mainLoopTimer;


// CPU freq = 32MHz
void Systick_Init(void) {
	
	// Setup SysTick Timer for 1 msec interrupts
	SysTick_Config((SystemCoreClock / 1000));
	
	// Setup software timers
	memset(&mainLoopTimer, 0, sizeof(mainLoopTimer));
	mainLoopTimer.top = 19;
	mainLoopTimer.enabled = 1;
}



void SysTick_Handler(void) {
	processSoftTimer16b(&mainLoopTimer);
	LCD_ProcessContrastBooster();
	Encoder_Process();
	Sound_Process();
}



