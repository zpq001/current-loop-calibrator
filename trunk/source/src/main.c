

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
#include "eeprom.h"

/*
	timer3 -> buzzer
	timer2 -> contrast
	timer1 -> DMA for waveform
*/



int main(void) {
	
    // Initialize system 
	hw_Setup_CPU_Clock();
	hw_Init_DMA();
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
	// Buzzer
	// TODO
	// Watchdog
	// TODO
    // GUI
    GUI_Init();
    // Wait a bit for greeting message
    DWT_DelayUs(500000);
    // EEPROM memory - restore settings
    if (EE_RestoreSettings()) {
        DAC_RestoreSettings();
	} else {
        // Show error message!
        LCD_Clear();
        LCD_PutStringXY(0,0,"Необходима");
        LCD_PutStringXY(0,1,"     калибровка!");
        // Wait a bit more
        DWT_DelayUs(500000);
	}
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
            
            LCD_CaptureKeyboard();
            ProcessButtons();
			Encoder_UpdateDelta();
			
/*
    if (buttons.action_hold & KEY_NUM2) {
        DAC_SaveSettings();
		__disable_irq();
		EE_SaveSettings();
		__enable_irq();
	}
    else if (buttons.action_hold & KEY_NUM1) {
		__disable_irq();
		EE_RestoreSettings();
		__enable_irq();
        DAC_RestoreSettings();
	}
	*/	
            GUI_Process();
		}
	}
}




