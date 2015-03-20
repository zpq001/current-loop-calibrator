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
#define WAVEFORM_BUFFER_SIZE	2000



static struct {
	uint32_t setting;		// [uA]
	uint32_t dac_code;
	uint8_t mode;
	uint8_t waveform;
	uint8_t period;			// [us]
	uint32_t wave_min;		// [uA]
	uint32_t wave_max;		// [uA]
} dac_state;

static calibration_t dac_calibration;
static uint16_t waveform_buffer[WAVEFORM_BUFFER_SIZE];

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
	DMA_PriCtrlStr.DMA_SourceBaseAddr = (uint32_t)&waveform_buffer;
	DMA_PriCtrlStr.DMA_DestBaseAddr = (uint32_t)(uint32_t)(&(MDR_DAC->DAC2_DATA));		// dest 
	DMA_PriCtrlStr.DMA_SourceIncSize = DMA_SourceIncHalfword;
	DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncNo ;
	DMA_PriCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_PriCtrlStr.DMA_Mode = DMA_Mode_Basic;								// 
	DMA_PriCtrlStr.DMA_CycleSize = WAVEFORM_BUFFER_SIZE;							// count of 16-bit shorts
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

void DAC_InitDMATimer(void) {
	TIMER_CntInitTypeDef sTIM_CntInit;
	
	// Initialize timer 2 counters
	TIMER_CntStructInit(&sTIM_CntInit);
	sTIM_CntInit.TIMER_Prescaler                = 31;			// CLK = F_CPU / (prescaler + 1)
	sTIM_CntInit.TIMER_Period                   = period_us;	// 1MHz / 100 = 10kHz
	TIMER_CntInit(MDR_TIMER1,&sTIM_CntInit);
	
	// Enable TIMER2 counter clock
	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);
	// Set requests to the DMA
	TIMER_DMACmd(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);
}

void DAC_SetDMATimerPeriod(uint32_t new_period) {
	MDR_TIMER1.ARR = new_period;
	if (MDR_TIMER1.CNT > new_period)
		MDR_TIMER1.CNT = 0;
}

void DAC_StartDMATimer(uint32_t period_us) {
	MDR_TIMER1.ARR = new_period;
	MDR_TIMER1.CNT = 0;
	TIMER_Cmd(MDR_TIMER1, ENABLE);
}

void DAC_StopDMATimer(void) {
	TIMER_Cmd(MDR_TIMER1, DISABLE);
}



// Sets DAC output to specified value [uA]
static void DAC_UpdateOutput(uint32_t value) {
	uint32_t temp32u;
	temp32u = GetCodeForValue(&dac_calibration, value);
	if (temp32u > 4095)
		temp32u = 4095;
	DAC2_SetData(temp32u);
}
	//LED_Set(LED_STATE, 1);

	
	
	
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
	dac_state.setting = 4000;
	dac_state.mode = DAC_MODE_CONST;
	dac_state.waveform = WAVE_MEANDR;
	dac_state.period = 5;
	dac_state.wave_min = 4000;
	dac_state.wave_max = 20000;
	// Generate waveform
	CreateSawWaveform(waveform_buffer, dac_state.wave_min, dac_state.wave_max, WAVEFORM_BUFFER_SIZE);
	
	DAC_UpdateOutput(dac_state.setting);
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



void DAC_SetSettingConst(uint32_t newValue) {
    dac_state.setting = new_value;
	DAC_UpdateOutput(dac_state.setting);
}

void DAC_SetSettingWaveMax(uint32_t value) {
	dac_state.wave_max = value;
	// Regenerate waveform
	CreateSawWaveform(waveform_buffer, dac_state.wave_min, dac_state.wave_max, WAVEFORM_BUFFER_SIZE);
}

void DAC_SetSettingWaveMin(uint32_t value) {
	dac_state.wave_min = value;
	// Regenerate waveform
	CreateSawWaveform(waveform_buffer, dac_state.wave_min, dac_state.wave_max, WAVEFORM_BUFFER_SIZE);
}

void DAC_SetWaveform(uint8_t newWaveForm) {
    dac_state.waveform = newWaveForm;
    // Restart DMA!
}

void DAC_SetPeriod(uint16_t new_period) {
    dac_state.period = new_period;
	if (dac_state.mode == DAC_MODE_WAVEFORM) {
		DAC_SetDMATimerPeriod(dac_state.period);
	}
}

void DAC_SetMode(uint8_t new_mode) {
	if (new_mode != dac_state.mode) {
		if (new_mode == DAC_MODE_WAVEFORM) {
			dac_state.mode = DAC_MODE_WAVEFORM;
			// Waveform is already generated
			DAC_StartDMATimer(dac_state.period);
			// TODO  Restart DMA
		} else {
			dac_state.mode = DAC_MODE_CONST;
			DAC_StopDMATimer();
			DAC_UpdateOutput(dac_state.setting);
		}
	}
}






uint32_t DAC_GetSettingConst(void) {
    return dac_state.setting;
}

uint32_t DAC_GetSettingWaveMax(void) {
    return dac_state.wave_max;
}

uint32_t DAC_GetSettingWaveMin(void) {
    return dac_state.wave_min;
}

uint8_t DAC_GetWaveform(void) {
    return dac_state.waveform;
}

uint16_t DAC_GetPeriod(void) {
    return DAC_Period;
}

uint8_t DAC_GetMode(void) {
	return dac_state.mode;
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



