/*******************************************************************
	Module sound
	
	Buzzer control

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_timer.h"
#include "sound.h"
#include "eeprom.h"
#include "sound_samples.h"


enum FsmStates {IDLE, START_TONE, PLAYING_TONE};



static uint8_t fsm_state;
static uint16_t ms_counter;
static const tone_t *melody_ptr;
static const tone_t *new_melody_ptr;
static uint8_t melody_index;
static uint8_t sound_enabled;

void Sound_Init(void) {
    
    TIMER_CntInitTypeDef sTIM_CntInit;
	TIMER_ChnInitTypeDef sTIM_ChnInit;
	TIMER_ChnOutInitTypeDef sTIM_ChnOutInit;
	PORT_InitTypeDef PORT_InitStructure;
	
	// Initialize timer 3 counter
	TIMER_CntStructInit(&sTIM_CntInit);
	sTIM_CntInit.TIMER_Prescaler                = 31;		// CLK = F_CPU / (prescaler + 1)
	sTIM_CntInit.TIMER_Period                   = 1000;		// 1MHz / 1000 = 1kHz
	TIMER_CntInit (MDR_TIMER3,&sTIM_CntInit);

	// Initialize timer 3 channel 1 - used for LCD contrast
	TIMER_ChnStructInit(&sTIM_ChnInit);
	sTIM_ChnInit.TIMER_CH_Mode                = TIMER_CH_MODE_PWM;
	sTIM_ChnInit.TIMER_CH_REF_Format          = TIMER_CH_REF_Format6;
	sTIM_ChnInit.TIMER_CH_CCR_UpdateMode      = TIMER_CH_CCR_Update_On_CNT_eq_0;
	sTIM_ChnInit.TIMER_CH_Number              = TIMER_CHANNEL1;			
	TIMER_ChnInit(MDR_TIMER3, &sTIM_ChnInit);
    
	// Initialize timer 3 channel 1 output
	TIMER_ChnOutStructInit(&sTIM_ChnOutInit);
	sTIM_ChnOutInit.TIMER_CH_DirOut_Polarity          = TIMER_CHOPolarity_NonInverted;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Source            = TIMER_CH_OutSrc_REF;
	sTIM_ChnOutInit.TIMER_CH_DirOut_Mode              = TIMER_CH_OutMode_Output;
	sTIM_ChnOutInit.TIMER_CH_Number                   = TIMER_CHANNEL1;
	TIMER_ChnOutInit(MDR_TIMER3, &sTIM_ChnOutInit);
	
	// Set contrast PWM duty cycle 0%
	MDR_TIMER3->CCR1 = 0;
    
	// Enable TIMER2 counter clock
	TIMER_BRGInit(MDR_TIMER3,TIMER_HCLKdiv1);

	// Enable TIMER2
	TIMER_Cmd(MDR_TIMER3,ENABLE);
    
    
    // Setup GPIO
    PORT_ResetBits(BUZZER_PORT, (1<<BUZZER_PIN));
    
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << BUZZER_PIN);
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_OE = PORT_OE_OUT;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(BUZZER_PORT, &PORT_InitStructure);   
    
    fsm_state = IDLE;
    new_melody_ptr = 0;
    sound_enabled = 1;
}

static void set_period(uint16_t period_us) {
    MDR_TIMER3->ARR = period_us;
}

static void set_output(uint8_t state) {
    if (state) 
        MDR_TIMER3->CCR1 = MDR_TIMER3->ARR >> 1;
    else
        MDR_TIMER3->CCR1 = 0;
}


void Sound_SetEnabled(uint8_t state) {
    sound_enabled = state;
}

uint8_t Sound_GetEnabled(void) {
    return sound_enabled;
}

void Sound_RestoreSetting(void) {
    sound_enabled = system_settings.sound_state;
}

void Sound_SaveSetting(void) {
    system_settings.sound_state = sound_enabled;
}


void Sound_Event(uint8_t event) {
    if (sound_enabled) {
        switch (event) {
            case SE_Start: 
                new_melody_ptr = _beep_KeyConfirm;
                break;
            case SE_KeyConfirm:
                new_melody_ptr = _beep_KeyConfirm;
                break;
            case SE_KeyReject:
                new_melody_ptr = _beep_KeyReject;
                break;
            case SE_EncoderConfirm:
                new_melody_ptr = _beep_EncoderConfirm;
                break;
            case SE_EncoderIllegal:
                new_melody_ptr = _beep_EncoderIllegal;
                break;
            case SE_SettingConfirm:
                new_melody_ptr = _melody_SettingConfirm;
                break;
            case SE_SettingIllegal:
                new_melody_ptr = _melody_SettingIllegal;
                break;
            case SE_CyclesDone:
                new_melody_ptr = _beep_SyclesDone;
                break;
        }
    }
}

// Should be called with period of 1ms by systick ISR
void Sound_Process(void) {
    uint8_t exit = 0;
    const tone_t *tone_ptr;
    // New event overrides current
    if (new_melody_ptr != 0) {
        fsm_state = IDLE;
    }
    while(!exit) {
        switch(fsm_state) {
            case IDLE:
                if (new_melody_ptr) {
                    melody_ptr = new_melody_ptr;
                    new_melody_ptr = 0;
                    melody_index = 0;
                    fsm_state = START_TONE;
                } else {
                    exit = 1;
                }
                break;
            case START_TONE:
                tone_ptr = &melody_ptr[melody_index++];
                if (tone_ptr->duration > 0) {
                    if (tone_ptr->tone_period > 0) {
                        set_period(tone_ptr->tone_period);
                        set_output(1);
                    } else {
                        set_output(0);  // Silence
                    }
                    ms_counter = tone_ptr->duration;
                    fsm_state = PLAYING_TONE;
                } else {
                    // End of melody
                    fsm_state = IDLE;
                    set_output(0);  // Silence
                }
                exit = 1;
                break;
            case PLAYING_TONE:
                if (--ms_counter == 0)
                    fsm_state = START_TONE;
                else
                    exit = 1;
                break;
        }
    }    
}





