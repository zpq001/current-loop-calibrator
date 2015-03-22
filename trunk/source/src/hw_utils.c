/***************************************************************//**
	@brief hw_utils
	
	Low-level functions for hardware setup and control
	
********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_dma.h"
#include "MDR32F9Qx_ssp.h"


extern DMA_CtrlDataTypeDef DMA_ControlTable[];
void DMA_CtrlDataInit(DMA_CtrlDataInitTypeDef *DMA_ctrl_data_ptr, DMA_CtrlDataTypeDef *DMA_ctrl_table_ptr);


// GPIO ports
#define PORTS_CLK       (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB |  \
                         RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD |  \
                         RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

// Peripheral blocks, used in design											
#define PERIPHERALS_CLK (RST_CLK_PCLK_SSP1 |    \
					     RST_CLK_PCLK_TIMER1 |  \
						 RST_CLK_PCLK_TIMER2 |  \
                         RST_CLK_PCLK_TIMER3 |  \
						 RST_CLK_PCLK_ADC |     \
                         RST_CLK_PCLK_DAC |     \
						 RST_CLK_PCLK_DMA |     \
                         RST_CLK_PCLK_COMP) | 	\
						 RST_CLK_PCLK_EEPROM



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


//-------------------------------------------------------//
/// @brief  Setup clocks to use HSI
//  HSI freq = 8MHz
//-------------------------------------------------------//
void hw_Switch_CPU_Clock_to_HSI(void) {
	// HCLK = HSI
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkHSI);
	// Disable HSE
	RST_CLK_HSEconfig(RST_CLK_HSE_OFF);
	// Disable PLL
	RST_CLK_CPU_PLLcmd(DISABLE);
}



// Globally initializes DMA controller
void my_DMA_GlobalInit(void)
{
	/* Check the CTRL_BASE_PTR initialisation */
	// MDR_DMA->ALT_CTRL_BASE_PTR is automatically updated by DMA itself and is accessible for read only
	MDR_DMA->CTRL_BASE_PTR = (uint32_t)DMA_ControlTable;
	/* DMA configuration register */
	//MDR_DMA->CFG = DMA_CFG_MASTER_ENABLE || 0 /*DMA_InitStruct->DMA_ProtCtrl*/;		// CHECKME
	MDR_DMA->CFG = DMA_CFG_MASTER_ENABLE | DMA_AHB_Privileged;
}



// Operates similar to SPL function DMA_Init(), but:
//	- Disables channel before init
//	- Does NOT start the channel
//  - Does not modify general DMA controller registers
//	- DMA_ChannelInitStruct.ProtCtrl is unused
void my_DMA_ChannelInit(uint8_t DMA_Channel, DMA_ChannelInitTypeDef* DMA_InitStruct)
{
	// Make sure channel is disabled
	MDR_DMA->CHNL_ENABLE_CLR = (1 << DMA_Channel);

	/* Primary Control Data Init */
	if (DMA_InitStruct->DMA_PriCtrlData)
	{
		DMA_CtrlDataInit(DMA_InitStruct->DMA_PriCtrlData, &DMA_ControlTable[DMA_Channel]);
	}

#if (DMA_AlternateData == 1)
	/* Alternate Control Data Init */
	if (DMA_InitStruct->DMA_AltCtrlData)
	{
		uint32_t ptr = (MDR_DMA->ALT_CTRL_BASE_PTR + (DMA_Channel * sizeof(DMA_CtrlDataTypeDef)));
		DMA_CtrlDataInit(DMA_InitStruct->DMA_AltCtrlData, (DMA_CtrlDataTypeDef *)ptr);
	}
#endif

	/* Burst mode */
	if (DMA_InitStruct->DMA_UseBurst == DMA_BurstSet)
	{
		MDR_DMA->CHNL_USEBURST_SET = (1 << DMA_Channel);
	}
	else
	{
		MDR_DMA->CHNL_USEBURST_CLR = (1 << DMA_Channel);
	}

	/* Channel mask clear - enable requests to channel */
	MDR_DMA->CHNL_REQ_MASK_CLR = (1 << DMA_Channel);
  
	/* Primary - Alternate control data structure selection */
	if (DMA_InitStruct->DMA_SelectDataStructure == DMA_CTRL_DATA_PRIMARY)
	{
		MDR_DMA->CHNL_PRI_ALT_CLR = (1 << DMA_Channel);       /* Use Primary */
	}
	else
	{
		MDR_DMA->CHNL_PRI_ALT_SET = (1 << DMA_Channel);       /* Use Alternate */
	}

	/* Channel priority set */
	if (DMA_InitStruct->DMA_Priority == DMA_Priority_High)
	{
		MDR_DMA->CHNL_PRIORITY_SET = (1 << DMA_Channel);      /* High priority */
	}
	else
	{
		MDR_DMA->CHNL_PRIORITY_CLR = (1 << DMA_Channel);      /* Default priority */
	}
}






//-----------------------------------------------------------------//
// Setup DMA
// HCLK = 32 MHz
//-----------------------------------------------------------------//
void hw_Init_DMA(void)
{
	// Reset all DMA settings
	DMA_DeInit();	
	
	// Vital for proper DMA IRQ function
	// Single requests from ADC?
	MDR_DMA->CHNL_REQ_MASK_SET = 0xFFFFFFFF;	// Disable all requests
	MDR_DMA->CHNL_USEBURST_SET = 0xFFFFFFFF;	// disable sreq[]
	
	// MDR32F9Qx false DMA requests workaround
	// This must be executed next to clock setup
	//RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP1 ,ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP2 ,ENABLE);
	//SSP_BRGInit(MDR_SSP1,SSP_HCLKdiv1);		
	//SSP_BRGInit(MDR_SSP2,SSP_HCLKdiv1);	
	MDR_SSP1->DMACR = 0;					// Reset false requests
	MDR_SSP2->DMACR = 0;
	//RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP1 ,DISABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP2 ,DISABLE);
	
	NVIC->ICPR[0] = 0xFFFFFFFF;		// Reset all pending interrupts
	NVIC->ICER[0] = 0xFFFFFFFF;		// Disable all interrupts
	
	my_DMA_GlobalInit();
	
	NVIC_EnableIRQ(DMA_IRQn);
	
}




