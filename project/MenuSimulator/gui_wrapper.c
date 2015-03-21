
#include <string.h>
#include <stdio.h>
#include "buttons.h"
#include "gui_wrapper.h"
#include "gui_top.h"
#include "adc.h"
#include "external_adc.h"
#include "dac.h"



buttons_t buttons;
int16_t encoder_delta;

struct {
    char *strings[4];    // + \0
    char str_data[4][21];
    uint8_t cursor_x;
    uint8_t cursor_y;
} lcd;


// Callback functions
cbLcdUpdatePtr updateLcdCallback;

static struct {
    uint32_t setting;		// [uA]
    uint32_t dac_code;
    uint8_t mode;
    uint8_t waveform;
    uint32_t period;		// [ms]
    uint32_t wave_min;		// [uA]
    uint32_t wave_max;		// [uA]
    uint32_t total_cycles;
    uint32_t current_cycle;
} dac_state;

//-----------------------------------//
// Callbacks top->GUI
void registerLcdUpdateCallback(cbLcdUpdatePtr fptr)
{
    updateLcdCallback = fptr;
}


void guiInitialize(void)
{
    // Default state after power-on
    dac_state.setting = 4000;
    dac_state.mode = DAC_MODE_CONST;
    dac_state.waveform = WAVE_MEANDR;
    dac_state.period = 1500;
    dac_state.wave_min = 4000;
    dac_state.wave_max = 20000;
    dac_state.total_cycles = 95684;
    dac_state.current_cycle = 87521;

    uint8_t i;
    for (i=0; i<4; i++)
    {
        lcd.strings[i] = lcd.str_data[i];
    }
    LCD_Clear();
    GUI_Init();
}

void guiButtonEvent(void)
{
    GUI_Process();
}

void guiUpdate(void)
{
    GUI_Process();
}


//-----------------------------------//
// Taps

void LCD_Clear(void) {
    uint8_t i;
    for (i=0; i<4; i++)
        snprintf(lcd.strings[i], 21, "                    ");
    lcd.cursor_x = 0;
    lcd.cursor_y = 0;
    updateLcdCallback((char**)lcd.strings);
}

void LCD_SetCursorPosition(uint8_t x, uint8_t y) {
    lcd.cursor_x = x;
    lcd.cursor_y = y;
}


void LCD_PutString(const char *data) {
    uint16_t len = strlen(data);
    char *tmp = lcd.strings[lcd.cursor_y];
    memcpy(&tmp[lcd.cursor_x], data, len);
    updateLcdCallback((char**)lcd.strings);
}

void LCD_InsertChars(const char *data, uint8_t count) {
    memcpy(&lcd.strings[lcd.cursor_y][lcd.cursor_x], data, count);
    updateLcdCallback((char**)lcd.strings);
}

void LCD_PutStringXY(uint8_t x, uint8_t y, const char *data) {
    LCD_SetCursorPosition(x, y);
    LCD_PutString(data);
    updateLcdCallback((char**)lcd.strings);
}

void LCD_InsertCharsXY(uint8_t x, uint8_t y, const char *data, uint8_t count) {
    LCD_SetCursorPosition(x, y);
    LCD_InsertChars(data, count);
    updateLcdCallback((char**)lcd.strings);
}



uint8_t ADC_GetLoopStatus(void) {
    return LOOP_OK;
}

uint32_t ADC_GetLoopCurrent(void) {
    return 0;
}

uint32_t ADC_GetLoopVoltage(void) {
    return 18562;
}


int32_t ExtADC_GetCurrent(void) {
    return 0;
}

uint8_t ExtADC_GetRange(void) {
    return 0;
}




void DAC_RestoreSettings(void) {
}

void DAC_SaveSettings(void) {
}



void DAC_SetSettingConst(uint32_t value) {
    dac_state.setting = value;
}

void DAC_SetSettingWaveMax(uint32_t value) {
    dac_state.wave_max = value;
}

void DAC_SetSettingWaveMin(uint32_t value) {
    dac_state.wave_min = value;
}

void DAC_SetWaveform(uint8_t newWaveForm) {
    dac_state.waveform = newWaveForm;
}

void DAC_SetPeriod(uint32_t new_period) {
    dac_state.period = new_period;
}

void DAC_SetMode(uint8_t new_mode) {
    if (new_mode != dac_state.mode) {
        if (new_mode == DAC_MODE_WAVEFORM) {
            dac_state.mode = DAC_MODE_WAVEFORM;

        } else {
            dac_state.mode = DAC_MODE_CONST;
        }
    }
}

void DAC_SetTotalCycles(uint32_t number) {
    dac_state.total_cycles = number;
    dac_state.current_cycle = number - 1;
}

void DAC_RestartCycles(void) {
    dac_state.current_cycle = 1;
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






