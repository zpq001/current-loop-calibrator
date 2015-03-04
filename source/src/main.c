

#include "hw_utils.h"
#include "dwt_delay.h"


int main(void) {
	
    // Initialize system 
	hw_Setup_CPU_Clock();
    DWT_Init();
    
    // Initialize software modules. 
    // All required HW setup is performed inside modules.
    
    // LCD and keyboard controller
    LCD_Init();
    
    
    
	
}
