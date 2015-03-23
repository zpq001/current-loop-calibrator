

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
	uint8_t system_settings_ok;
	uint8_t settings_ok;
	
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
	
	// Get mode of operation
	LCD_CaptureKeyboard();	
	device_mode = (GetRawButtonState() & KEY_OUTPUT_CTRL) ? MODE_CALIBRATION : MODE_NORMAL;
	// Restore system settings
	system_settings_ok = EE_RestoreSystemSettings((device_mode == MODE_CALIBRATION));
	if (system_settings_ok) {
		// Apply system settings:
		DAC_ApplyCalibration();
		// Contrast
		// ADC
		// extADC
	}
	// Restore settings
	if (device_mode == MODE_NORMAL) {
		settings_ok = EE_RestoreSettings();
		if (settings_ok) {
			// Apply settings
			DAC_RestoreSettings();
			// other modules
		}
	}
	
	// Wait a bit for greeting message
    DWT_DelayUs(500000);
	
	if ((system_settings_ok == 0) && (device_mode == MODE_NORMAL)) {
		// Show error message!
		LCD_Clear();
		LCD_PutStringXY(0,0,"Необходима");
		LCD_PutStringXY(0,1,"     калибровка!");
		// Wait a bit more
		DWT_DelayUs(1000000);
	}
	
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




