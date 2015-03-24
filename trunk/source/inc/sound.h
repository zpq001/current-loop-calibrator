
#define BUZZER_PORT	    MDR_PORTC
#define BUZZER_PIN	    2


enum SoundEvents {
    SE_Start,
    SE_KeyConfirm,
    SE_KeyIllegal,
    SE_EncoderConfirm,
    SE_EncoderIllegal,
    SE_SettingConfirm,
    SE_SettingIllegal,
    SE_CyclesDone
};

typedef struct {
	uint16_t tone_period;		// in 1 us gradation	
	uint16_t duration;			// in 1 ms gradation
} tone_t;



void Sound_Init(void);
uint8_t Sound_SetEnabled(uint8_t state);
uint8_t Sound_GetEnabled(void);
void Sound_RestoreSetting(void);
void Sound_SaveSetting(void);
void Sound_Event(uint8_t event);
void Sound_Process(void);
