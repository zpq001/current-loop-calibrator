/*******************************************************************
	Module systick
	
	LCD contrast DC-DC control

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_timer.h"
#include "lcd_contrast.h"

static uint8_t contrastSetting = 10;	// 0 to 20

/*
static void enableCPWM(void) {
	MDR_TIMER2->CCR3 = MDR_TIMER2->ARR >> 1;
}


static void disableCPWM(void) {
	MDR_TIMER2->CCR3 = 0;	
}
*/

void LCD_InitContrastBooster(void) {

	TIMER_CntInitTypeDef sTIM_CntInit;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;
	PORT_InitTypeDef PORT_InitStructure;
	
	// Initialize timer 2 counter
	TIMER_CntStructInit(&sTIM_CntInit);
	sTIM_CntInit.TIMER_Prescaler                = 31;		// CLK = F_CPU / (prescaler + 1)
	sTIM_CntInit.TIMER_Period                   = 1000;		// 1MHz / 100 = 10kHz
	TIMER_CntInit (MDR_TIMER2,&sTIM_CntInit);

	// Initialize timer 2 channel 3 - used for LCD contrast
	TIMER_ChnStructInit(&sTIM_ChnInit);
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode      = TIMER_CH_CCR_Update_On_CNT_eq_0;
	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL3;			
	TIMER_ChnInit(MDR_TIMER2, &sTIM_ChnInit);
	/*
	// Initialize timer 2 channel 2 - used for interrupt generation
	TIMER_ChnStructInit(&sTIM_ChnInit);
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format1;	// REF output = 1 when CNT == CCR
	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL2;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode      = TIMER_CH_CCR_Update_Immediately;
	TIMER_ChnInit(MDR_TIMER2, &sTIM_ChnInit);
	*/
	// Initialize timer 2 channel 3 output
	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);
	sTIM_ChnOutInit.TIMER_CH_NegOut_Polarity          = TIMER_CHOPolarity_NonInverted;
	sTIM_ChnOutInit.TIMER_CH_NegOut_Source            = TIMER_CH_OutSrc_REF;
	sTIM_ChnOutInit.TIMER_CH_NegOut_Mode              = TIMER_CH_OutMode_Output;
	sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL3;
	TIMER_ChnOutInit(MDR_TIMER2, &sTIM_ChnOutInit);
	
	// Set contrast PWM duty cycle 50%
	MDR_TIMER2->CCR3 = MDR_TIMER2->ARR >> 1;
	// Set default CCR for interrupt generation
	//MDR_TIMER2->CCR2 = 0;
	
	// Enable interrupts
	//TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CCR_REF_CH2, ENABLE);
	
	// Enable TIMER2 counter clock
	TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	// Enable TIMER2
	TIMER_Cmd(MDR_TIMER2,ENABLE);
	
	
    // Setup GPIO
    PORT_ResetBits(CPWM_PORT, (1<<CPWM_PIN));
    
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << CPWM_PIN);
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_OE = PORT_OE_OUT;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(CPWM_PORT, &PORT_InitStructure);
}

void LCD_ProcessContrastBooster(void) {
	uint16_t temp16u;
	
	TIMER_SetCntAutoreload(MDR_TIMER2, 900);
	// Set contrast PWM duty cycle 50%
	MDR_TIMER2->CCR3 = MDR_TIMER2->ARR >> 1; 
	
	
	
	temp16u = 36 - contrastSetting;
	temp16u *= 100;
	
}







