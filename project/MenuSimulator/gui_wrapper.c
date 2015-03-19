
#include <string.h>
#include <stdio.h>
#include "buttons.h"
#include "gui_wrapper.h"
#include "gui_top.h"
#include "adc.h"
#include "external_adc.h"
#include "dac.h"
buttons_t buttons;
struct {
    char *strings[4];    // + \0
    char str_data[4][21];
    uint8_t cursor_x;
    uint8_t cursor_y;
} lcd;


// Callback functions
cbLcdUpdatePtr updateLcdCallback;

static uint32_t DAC_SettingConst;
static uint8_t DAC_WaveForm;
static uint16_t DAC_Period;

//-----------------------------------//
// Callbacks top->GUI
void registerLcdUpdateCallback(cbLcdUpdatePtr fptr)
{
    updateLcdCallback = fptr;
}


void guiInitialize(void)
{
    DAC_SettingConst = 16800;
    DAC_WaveForm = WAVE_MEANDR;
    DAC_Period = 345;

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
    memset(&buttons, 0, sizeof(buttons));
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

uint32_t DAC_GetSettingConst(void) {
    return DAC_SettingConst;
}

uint32_t DAC_GetSettingAlternHigh(void) {
    return 20000;
}

uint32_t DAC_GetSettingAlternLow(void) {
    return 4000;
}

void DAC_SetSettingConst(uint32_t newValue) {
    DAC_SettingConst = newValue;
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

}

void DAC_SetConstantMode(void) {

}






