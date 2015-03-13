
#include <string.h>
#include <stdio.h>
#include "buttons.h"
#include "gui_wrapper.h"
#include "gui_top.h"
#include "adc.h"
#include "external_adc.h"

buttons_t buttons;
struct {
    char *strings[4];    // + \0
    char str_data[4][21];
    uint8_t cursor_x;
    uint8_t cursor_y;
} lcd;


// Callback functions
cbLcdUpdatePtr updateLcdCallback;

//-----------------------------------//
// Callbacks top->GUI
void registerLcdUpdateCallback(cbLcdUpdatePtr fptr)
{
    updateLcdCallback = fptr;
}


void guiInitialize(void)
{
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
    return 0;
}


int32_t ExtADC_GetCurrent(void) {
    return 0;
}

uint8_t ExtADC_GetRange(void) {
    return 0;
}










