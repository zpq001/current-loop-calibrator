/***************************************************************//**
	@brief hw_utils
	
	Low-level functions for hardware setup and control
	
********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"

// GPIO ports
#define PORTS_CLK       (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB |  \
                         RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD |  \
                         RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

// Peripheral blocks, used in design											
#define PERIPHERALS_CLK (RST_CLK_PCLK_SSP1 |    \
						 RST_CLK_PCLK_TIMER2 |  \
                         RST_CLK_PCLK_TIMER3 |  \
						 RST_CLK_PCLK_ADC |     \
                         RST_CLK_PCLK_DAC |     \
                         RST_CLK_PCLK_COMP)



//-------------------------------------------------------//
/// @brief  Setup clocks
//  HSE freq = 8MHz
//  CPU freq = 32MHz
//-------------------------------------------------------//
void hw_Setup_CPU_Clock(void) {

    // Enable HSE
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	if (RST_CLK_HSEstatus() != SUCCESS)	{
		while (1) {}	// Trap
	}
    
	// Setup CPU PLL and CPU_C1_SEL
	// CPU_C1 = HSE,	PLL = x4
	RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul4);
	RST_CLK_CPU_PLLcmd(ENABLE);
	if (RST_CLK_CPU_PLLstatus() != SUCCESS)	{
		while (1) {}	// Trap
	}
    
	// Setup CPU_C2 and CPU_C3
	// CPU_C3 = CPU_C2
	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
	// CPU_C2 = CPU PLL output
	RST_CLK_CPU_PLLuse(ENABLE);
	// Switch to CPU_C3
	// HCLK = CPU_C3
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
    
	// Setup ADC clock
	// ADC_C2 = CPU_C1
	RST_CLK_ADCclkSelection(RST_CLK_ADCclkCPU_C1);
	// ADC_C3 = ADC_C2
	RST_CLK_ADCclkPrescaler(RST_CLK_ADCclkDIV1);
	// Enable ADC_CLK
	RST_CLK_ADCclkEnable(ENABLE);
	
	// Update system clock variable
	SystemCoreClockUpdate();
	
	// Enable clock on ports
	RST_CLK_PCLKcmd(PORTS_CLK, ENABLE);
	// Enable clock on peripheral blocks used in design
	RST_CLK_PCLKcmd(PERIPHERALS_CLK, ENABLE);

}






