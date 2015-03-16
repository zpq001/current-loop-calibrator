

#include "hw_utils.h"
#include "dwt_delay.h"
#include "lcd_melt20s4.h"
#include "lcd_contrast.h"
#include "buttons.h"
#include "led.h"
#include "encoder.h"
#include "dac.h"
#include "systick.h"
#include "adc.h"
#include "external_adc.h"
#include "power_monitor.h"
#include "gui_top.h"



int main(void) {
	
    // Initialize system 
	hw_Setup_CPU_Clock();
    DWT_Init();
    
    // Initialize software modules. 
    // All required HW setup is performed inside modules.
    
    // LCD and keyboard controller
    LCD_Init();
	// LCD contrast DC-DC controller
	LCD_InitContrastBooster();
    // Button events driver
    InitButtons();
    // LED driver
    LED_Init();
    // Incremental encoder driver
    Encoder_Init();
    // ADC driver
    ADC_Initialize();
    // SPI ADC driver
    ExtADC_Initialize();
	// DAC driver
    DAC_Initialize();
    // EEPROM memory
    // TODO
	// Buzzer
	// TODO
    // Restore settings
    // TODO
    // GUI
    GUI_Init();
	// Power supply monitor
	PowerMonitor_Init();
	// Start ISR-based syncronizer
	Systick_Init();
	
	while (1) {
		// Syncronize
		if (mainLoopTimer.flags.ovfl) {
			__disable_irq();
			mainLoopTimer.flags.ovfl = 0;
			__enable_irq();
			
			ADC_UpdateLoopVoltage();
			ADC_UpdateLoopCurrent();
			ADC_UpdateLoopMonitor();
            ExtADC_UpdateCurrent();
            
            //LCD_CaptureKeyboard();
            //ProcessButtons();
		
            //GUI_Process();
		}
	}
}




