/*******************************************************************
	Module dac.c
	
		Low-level functions for DAC


Calibration sequence:

	DAC_Initialize();
	// Point 1
	temp32u = DAC_GetCalibrationPoint(1);
	DAC_SetCurrent(temp32);
	... user enters actual current value, saved in meaValue ...
	DAC_SaveCalibrationPoint(1, meaValue);
	ADC_SaveCalibrationPoint(1, meaValue);
	// Point 2
	temp32u = DAC_GetCalibrationPoint(1);
	DAC_SetCurrent(temp32);
	... user enters actual current value, saved in meaValue ...
	DAC_SaveCalibrationPoint(2, meaValue);
	ADC_SaveCalibrationPoint(2, meaValue);
	// Done!
	DAC_Calibrate();

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_dac.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_dma.h"
#include "MDR32F9Qx_timer.h"
#include "hw_utils.h"
#include "linear_calibration.h"
#include "dac.h"
#include "led.h"

#define DEFAULT_OFF_VALUE		4000
#define DAC_BUFFER_SIZE			2000

static struct {
	uint32_t setting;
	uint32_t dac_code;
	uint8_t isEnabled;
} dac_state;

static calibration_t dac_calibration;
static uint32_t DAC_SettingConst;
static uint8_t DAC_WaveForm;
static uint16_t DAC_Period;
static uint16_t WaveFormBuffer[DAC_BUFFER_SIZE];

static uint32_t saved_TCB[3]; 	// Saved DMA configuration for fast reload in ISR
	

// Located in hw_utils
void my_DMA_ChannelInit(uint8_t DMA_Channel, DMA_ChannelInitTypeDef* DMA_InitStruct);
extern DMA_CtrlDataTypeDef DMA_ControlTable[];


// DMA channel configuration 
static void DAC_init_DMA(void)
{
	DMA_ChannelInitTypeDef DMA_InitStr;
	DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
	uint32_t *tcb_ptr;
	
	// Setup Primary Control Data 
	DMA_PriCtrlStr.DMA_SourceBaseAddr = (uint32_t)&WaveFormBuffer;
	DMA_PriCtrlStr.DMA_DestBaseAddr = (uint32_t)(uint32_t)(&(MDR_DAC->DAC2_DATA));		// dest 
	DMA_PriCtrlStr.DMA_SourceIncSize = DMA_SourceIncHalfword;
	DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncNo ;
	DMA_PriCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_PriCtrlStr.DMA_Mode = DMA_Mode_Basic;								// 
	DMA_PriCtrlStr.DMA_CycleSize = DAC_BUFFER_SIZE;							// count of 16-bit shorts
	DMA_PriCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
	DMA_PriCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;				// ?
	DMA_PriCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;					// ?
	
	// Setup Channel Structure
	DMA_InitStr.DMA_PriCtrlData = &DMA_PriCtrlStr;
	DMA_InitStr.DMA_AltCtrlData = 0;										// Not used
	DMA_InitStr.DMA_ProtCtrl = 0;											// Not used
	DMA_InitStr.DMA_Priority = DMA_Priority_High ;
	DMA_InitStr.DMA_UseBurst = DMA_BurstClear;								// Enable single words trasfer
	DMA_InitStr.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;
	
	// Init DMA channel
	my_DMA_ChannelInit(DMA_Channel_TIM1, &DMA_InitStr);
	
	// Save created RX UART DMA control block for reinit in DMA ISR
	tcb_ptr = (uint32_t*)&DMA_ControlTable[DMA_Channel_TIM1];
	saved_TCB[0] = *tcb_ptr++;
	saved_TCB[1] = *tcb_ptr++;
	saved_TCB[2] = *tcb_ptr;
}

void DAC_StartDMATimer(uint32_t period_us) {
	TIMER_CntInitTypeDef sTIM_CntInit;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
//	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;
	
	TIMER_DeInit(MDR_TIMER1);
	
	// Initialize timer 2 counter
	TIMER_CntStructInit(&sTIM_CntInit);
	sTIM_CntInit.TIMER_Prescaler                = 31;			// CLK = F_CPU / (prescaler + 1)
	sTIM_CntInit.TIMER_Period                   = period_us;	// 1MHz / 100 = 10kHz
	TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);

	// Initialize timer channel 1 - used for DMA requests generation
	TIMER_ChnStructInit(&sTIM_ChnInit);
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format1;	// REF output = 1 when CNT == CCR
	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL1;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode      = TIMER_CH_CCR_Update_Immediately;
	TIMER_ChnInit(MDR_TIMER1, &sTIM_ChnInit);
		
	// Set default CCR for DMA requests generation
	MDR_TIMER1->CCR1 = 10;
	
	// Enable interrupts
	//TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CCR_REF_CH2, ENABLE);
	
	// Enable TIMER2 counter clock
	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	// Enable TIMER2
	TIMER_Cmd(MDR_TIMER1,ENABLE);
	
	// Set requests to the DMA
	TIMER_DMACmd(MDR_TIMER1, TIMER_STATUS_CCR_REF_CH1, ENABLE);
}

void DAC_StopDMATimer(void) {
	TIMER_Cmd(MDR_TIMER1, DISABLE);
}




static void DAC_UpdateState(void) {
	uint32_t temp32u;
	if (dac_state.isEnabled) {
		temp32u = GetCodeForValue(&dac_calibration, dac_state.setting);
		if (temp32u > 4095)
			temp32u = 4095;
		DAC2_SetData(temp32u);
		dac_state.dac_code = temp32u;
		LED_Set(LED_STATE, 1);
	} else {
		temp32u = GetCodeForValue(&dac_calibration, DEFAULT_OFF_VALUE);
		DAC2_SetData(temp32u);
		dac_state.dac_code = DEFAULT_OFF_VALUE;
		LED_Set(LED_STATE, 0);
	}
}


void DAC_Initialize(void) {
	uint16_t temp16u;
    PORT_InitTypeDef PORT_InitStructure;
	
    // Setup GPIO
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << DAC_OUTPUT_PIN);
	PORT_Init(DAC_PORT, &PORT_InitStructure);
	
	// Setup DAC
    DAC2_Init(DAC2_REF);
    DAC2_Cmd(ENABLE);
    DAC2_SetData(0);  
	
	// Setup timer
	
	// Setup DMA
	DAC_init_DMA();
	// Enable RX DMA channel
	DMA_Cmd(DMA_Channel_TIM1, ENABLE);	
	
	// Default calibration
	dac_calibration.point1.value = 4000;
	dac_calibration.point1.code = 655;
	dac_calibration.point2.value = 20000;
	dac_calibration.point2.code = 3276;
    dac_calibration.scale = 10000L;
	CalculateCoefficients(&dac_calibration);
	
	// Default state after power-on
	dac_state.isEnabled = 1;
	dac_state.setting = 4000;
	
    DAC_WaveForm = WAVE_MEANDR;
    DAC_Period = 5;
	
	DAC_UpdateState();
	
	// Fill waveform
	for (temp16u=0; temp16u < 2000; temp16u++)
	{
		WaveFormBuffer[temp16u] = 1000 + temp16u;		// SAW
	}
}


// Sets DAC output to specified value [uA]
void DAC_SetCurrent(uint32_t new_value) {
	dac_state.setting = new_value;
	DAC_UpdateState();
}


// Enables or disables DAC output
void DAC_ToggleEnabled(void) {
	dac_state.isEnabled = !dac_state.isEnabled;
	DAC_UpdateState();
}


uint32_t DAC_GetCalibrationPoint(uint8_t pointNumber) {
	uint32_t temp32u;
	if (pointNumber == 1)
		temp32u = 4000;
	else
		temp32u = 20000;
	return temp32u;
}


void DAC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue) {
	calibration_point_t *p = (pointNum == 1) ? &dac_calibration.point1 : &dac_calibration.point2;
	p->value = measuredValue;
	p->code = dac_state.dac_code;
}


void DAC_Calibrate(void) {
	CalculateCoefficients(&dac_calibration);
	DAC_UpdateState();
}


void DAC_ApplyCalibration(calibration_t *c) {           // FIXME
	dac_calibration.point1.value = c->point1.value;
	dac_calibration.point1.code = c->point1.code;
	dac_calibration.point2.value = c->point2.value;
	dac_calibration.point2.code = c->point2.code;
	CalculateCoefficients(&dac_calibration);
	DAC_UpdateState();
}


void DAC_SaveCalibration(calibration_t *points) {           // FIXME
	points->point1.value = dac_calibration.point1.value;
	points->point1.code = dac_calibration.point1.code;
	points->point2.value = dac_calibration.point2.value;
	points->point2.code = dac_calibration.point2.code;
}



uint32_t DAC_GetSettingConst(void) {
    return dac_state.setting;
}

uint32_t DAC_GetSettingAlternHigh(void) {
    return 20000;
}

uint32_t DAC_GetSettingAlternLow(void) {
    return 4000;
}

void DAC_SetSettingConst(uint32_t newValue) {
    DAC_SetCurrent(newValue);
}



uint8_t DAC_GetWaveform(void) {
    return DAC_WaveForm;
}

void DAC_SetWaveform(uint8_t newWaveForm) {
    DAC_WaveForm = newWaveForm;
    // Restart DMA!
}

uint16_t DAC_GetPeriod(void) {
    return DAC_Period;
}

void DAC_SetPeriod(uint16_t newPeriod) {
    DAC_Period = newPeriod;
}

void DAC_SetWaveformMode(void) {
	DAC_StartDMATimer(500);
}

void DAC_SetConstantMode(void) {
	DAC_StopDMATimer();
	DAC_UpdateState();
}





void DMA_IRQHandler(void)
{
	uint32_t *tcb_ptr;
	
	// UART 1 DMA transfer complete
	if ((MDR_DMA->CHNL_ENABLE_SET & (1<<DMA_Channel_TIM1)) == 0)
	{
		// Reload TCB
		tcb_ptr = (uint32_t*)&DMA_ControlTable[DMA_Channel_TIM1];
		*tcb_ptr++ = saved_TCB[0];
		*tcb_ptr++ = saved_TCB[1];
		*tcb_ptr = saved_TCB[2];
		// Restart channel
		MDR_DMA->CHNL_ENABLE_SET = (1 << DMA_Channel_TIM1);
	}

	
	// Error handling
	if (DMA_GetFlagStatus(0, DMA_FLAG_DMA_ERR) == SET)
	{
		DMA_ClearError();	// normally this should not happen
	}

}



