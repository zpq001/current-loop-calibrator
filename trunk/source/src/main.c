

#include "hw_utils.h"
#include "dwt_delay.h"
#include "lcd_melt20s4.h"
#include "buttons.h"
#include "led.h"
#include "encoder.h"
#include "dac.h"


int main(void) {
	
    // Initialize system 
	hw_Setup_CPU_Clock();
    DWT_Init();
    
    // Initialize software modules. 
    // All required HW setup is performed inside modules.
    
    // LCD and keyboard controller
    LCD_Init();
    // Button events driver
    InitButtons();
    // LED driver
    LED_Init();
    // Incremental encoder driver
    Encoder_Init();
    // ADC driver
    // TODO
    // DAC driver
    DAC_Initialize();
    // SPI ADC driver
    // TODO
	
}
