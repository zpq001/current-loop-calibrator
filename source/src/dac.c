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
#include "eeprom.h"

#define WAVEFORM_BUFFER_SIZE	1000



static struct {
	uint32_t setting[PROFILE_COUNT];	// [uA]
	uint32_t dac_code;
	uint8_t profile;
	uint8_t mode;
	uint8_t waveform;
	uint32_t period;		// [ms]
	uint32_t wave_min;		// [uA]
	uint32_t wave_max;		// [uA]
	uint32_t total_cycles;
	uint32_t current_cycle;
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
	DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncNo;
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
	TIMER_DeInit(MDR_TIMER1);
	
	// Initialize timer 2 counters
	TIMER_CntStructInit(&sTIM_CntInit);
	sTIM_CntInit.TIMER_Prescaler                = 31;			// CLK = F_CPU / (prescaler + 1)
	sTIM_CntInit.TIMER_Period                   = 10;			// anything different from CNT, which is 0
	TIMER_CntInit(MDR_TIMER1,&sTIM_CntInit);
	
	// Enable TIMER2 counter clock
	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);
	// Set requests to the DMA
	TIMER_DMACmd(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE); 
}

static void DAC_RestartDMA(void) {
	uint32_t *tcb_ptr;
	// Reload TCB
	tcb_ptr = (uint32_t*)&DMA_ControlTable[DMA_Channel_TIM1];
	*tcb_ptr++ = saved_TCB[0];
	*tcb_ptr++ = saved_TCB[1];
	*tcb_ptr = saved_TCB[2];
	// Restart channel
	//MDR_DMA->CHNL_ENABLE_SET = (1 << DMA_Channel_TIM1);
}

//[ms]
void DAC_SetDMATimerPeriod(uint32_t new_period) {
	new_period *= 1000;
	new_period /= WAVEFORM_BUFFER_SIZE;
	MDR_TIMER1->ARR = new_period;
	if (MDR_TIMER1->CNT > new_period)
		MDR_TIMER1->CNT = 0;
}

void DAC_StartDMATimer(void) {
	MDR_TIMER1->CNT = 0;
	TIMER_Cmd(MDR_TIMER1, ENABLE);
}

void DAC_StopDMATimer(void) {
	TIMER_Cmd(MDR_TIMER1, DISABLE);
}



// Sets DAC output to specified value [uA]
void DAC_UpdateOutput(uint32_t value) {
	uint32_t temp32u;
	temp32u = GetCodeForValue(&dac_calibration, value);
	if (temp32u > 4095)
		temp32u = 4095;
	DAC2_SetData(temp32u);
	dac_state.dac_code = temp32u;	// save for calibration
}


static void DAC_GenerateWaveform(void) {
	uint16_t index;
	uint16_t min_code = GetCodeForValue(&dac_calibration, dac_state.wave_min);
	uint16_t max_code = GetCodeForValue(&dac_calibration, dac_state.wave_max);
	//if (dac_state.mode == DAC_MODE_WAVEFORM) {
	//	DAC_StopDMATimer();
	//}
	// Regenerate waveform
	switch (dac_state.waveform) {
		case WAVE_MEANDR:
			for (index = 0; index < WAVEFORM_BUFFER_SIZE; index++)
				waveform_buffer[index] = (index < WAVEFORM_BUFFER_SIZE/2) ? min_code : max_code;
		break;
		case WAVE_SAW_DIRECT:
			CreateSawWaveform(waveform_buffer, min_code, max_code, WAVEFORM_BUFFER_SIZE);
		break;
		case WAVE_SAW_REVERSED:
			CreateSawWaveform(waveform_buffer, max_code, min_code, WAVEFORM_BUFFER_SIZE);
		break;
	}
	//if (dac_state.mode == DAC_MODE_WAVEFORM) {
	//	DAC_StartDMATimer();
	//}
}
	
	
	
void DAC_Initialize(void) {
	uint8_t i;
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
	DAC_InitDMATimer();
	
	// Setup DMA
	DAC_init_DMA();
	// Enable RX DMA channel
	DMA_Cmd(DMA_Channel_TIM1, ENABLE);
	

	// Default state after power-on
	for (i=0; i<PROFILE_COUNT; i++)
		dac_state.setting[i] = 4000;
	dac_state.profile = 0;
	dac_state.mode = DAC_MODE_CONST;
	dac_state.waveform = WAVE_MEANDR;
	dac_state.period = 1000;
	dac_state.wave_min = 0000;
	dac_state.wave_max = 20000;
	dac_state.total_cycles = 10;
	dac_state.current_cycle = 1;
	
	// Default calibration
	dac_calibration.point1.value = 4000;
	dac_calibration.point1.code = 655;
	dac_calibration.point2.value = 20000;
	dac_calibration.point2.code = 3276;
    dac_calibration.scale = 10000L;
	
	CalculateCoefficients(&dac_calibration);
	DAC_GenerateWaveform();
	DAC_SetDMATimerPeriod(dac_state.period);
	DAC_UpdateOutput(dac_state.setting[dac_state.profile]);
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
	DAC_UpdateOutput(dac_state.setting[dac_state.profile]);
}

/*
void DAC_ApplyCalibration(calibration_t *c) {           // FIXME
	dac_calibration.point1.value = c->point1.value;
	dac_calibration.point1.code = c->point1.code;
	dac_calibration.point2.value = c->point2.value;
	dac_calibration.point2.code = c->point2.code;
	CalculateCoefficients(&dac_calibration);
	DAC_UpdateOutput(dac_state.setting[dac_state.profile]);
}


void DAC_SaveCalibration(calibration_t *points) {           // FIXME
	points->point1.value = dac_calibration.point1.value;
	points->point1.code = dac_calibration.point1.code;
	points->point2.value = dac_calibration.point2.value;
	points->point2.code = dac_calibration.point2.code;
}
*/

void DAC_RestoreSettings(void) {
	uint8_t i;
	// State
	for (i=0; i<PROFILE_COUNT; i++)
		dac_state.setting[i] = settings.dac.setting[i];
	dac_state.profile	     = settings.dac.profile;
	dac_state.waveform 		 = settings.dac.waveform;
	dac_state.period 		 = settings.dac.period;
	dac_state.wave_min 		 = settings.dac.wave_min;
	dac_state.wave_max 		 = settings.dac.wave_max;
	dac_state.total_cycles 	 = settings.dac.total_cycles;
	// Calibration
	dac_calibration.point1.value 	= settings.dac.calibration_point1.value;
	dac_calibration.point1.code 	= settings.dac.calibration_point1.code;
	dac_calibration.point2.value 	= settings.dac.calibration_point2.value;
	dac_calibration.point2.code 	= settings.dac.calibration_point2.code;
	CalculateCoefficients(&dac_calibration);
	DAC_GenerateWaveform();
	DAC_SetDMATimerPeriod(dac_state.period);
	DAC_UpdateOutput(dac_state.setting[dac_state.profile]);
}

void DAC_SaveSettings(void) {
	uint8_t i;
	// State
	for (i=0; i<PROFILE_COUNT; i++)
		settings.dac.setting[i] = dac_state.setting[i]; 	
	settings.dac.profile 		= dac_state.profile;
	settings.dac.waveform 		= dac_state.waveform; 	
	settings.dac.period 		= dac_state.period; 		
	settings.dac.wave_min 		= dac_state.wave_min; 	
	settings.dac.wave_max 		= dac_state.wave_max; 	
	settings.dac.total_cycles 	= dac_state.total_cycles; 
	// Calibration
	settings.dac.calibration_point1.value =  dac_calibration.point1.value;
	settings.dac.calibration_point1.code  =  dac_calibration.point1.code;
	settings.dac.calibration_point2.value =  dac_calibration.point2.value;
	settings.dac.calibration_point2.code  =  dac_calibration.point2.code;
}



void DAC_SetSettingConst(uint32_t value) {
    dac_state.setting[dac_state.profile] = value;
	if (dac_state.mode == DAC_MODE_CONST) {
		DAC_UpdateOutput(dac_state.setting[dac_state.profile]);
	}
}

void DAC_SetProfile(int16_t num) {
	if (num >= PROFILE_COUNT)
		num = 0;
	else if (num < 0)
		num = PROFILE_COUNT - 1;
	dac_state.profile = num;
		if (dac_state.mode == DAC_MODE_CONST) {
		DAC_UpdateOutput(dac_state.setting[dac_state.profile]);
	}
}

void DAC_SetSettingWaveMax(uint32_t value) {
	dac_state.wave_max = value;
	//DAC_StopDMATimer();
	DAC_GenerateWaveform();
	DAC_RestartDMA();
	//DAC_StartDMATimer();	// CHECKME
}

void DAC_SetSettingWaveMin(uint32_t value) {
	dac_state.wave_min = value;
	//DAC_StopDMATimer();
	DAC_GenerateWaveform();
	DAC_RestartDMA();
	//DAC_StartDMATimer();
}

void DAC_SetWaveform(uint8_t newWaveForm) {
    dac_state.waveform = newWaveForm;
	//DAC_StopDMATimer();
    DAC_GenerateWaveform();
	DAC_RestartDMA();
	//DAC_StartDMATimer();
}

void DAC_SetPeriod(uint32_t new_period) {
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
			DAC_RestartDMA();
			dac_state.current_cycle = 1;
			DAC_StartDMATimer();
		} else {
			dac_state.mode = DAC_MODE_CONST;
			DAC_StopDMATimer();
			DAC_UpdateOutput(dac_state.setting[dac_state.profile]);
		}
	}
}



void DAC_SetTotalCycles(uint32_t number) {
	DAC_StopDMATimer();
	DAC_RestartDMA();
	dac_state.total_cycles = (number == 0) ? 1 : number;
	dac_state.current_cycle = 1;
	DAC_StartDMATimer();
}

void DAC_RestartCycles(void) {
	DAC_StopDMATimer();
	DAC_RestartDMA();
	dac_state.current_cycle = 1;
	DAC_StartDMATimer();
}



uint32_t DAC_GetSettingConst(void) {
    return dac_state.setting[dac_state.profile];
}

uint8_t DAC_GetActiveProfile(void) {
	return dac_state.profile;
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
    return dac_state.period;
}

uint8_t DAC_GetMode(void) {
	return dac_state.mode;
}

uint32_t DAC_GetTotalCycles(void) {
	return dac_state.total_cycles;
}

uint32_t DAC_GetCurrentCycle(void) {
	return dac_state.current_cycle;
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
		
		if (dac_state.current_cycle < dac_state.total_cycles) {
			dac_state.current_cycle++;
		} else {
			TIMER_Cmd(MDR_TIMER1, DISABLE);
		}
	}

	
	// Error handling
	if (DMA_GetFlagStatus(0, DMA_FLAG_DMA_ERR) == SET)
	{
		DMA_ClearError();	// normally this should not happen
	}

}



