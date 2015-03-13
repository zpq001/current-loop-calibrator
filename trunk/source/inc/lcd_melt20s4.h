
#include "MDR32F9Qx_config.h"


#define LCD_PORT    MDR_PORTA
#define LCD_DB4     0
#define LCD_DB5     1   
#define LCD_DB6     2
#define LCD_DB7     3
#define LCD_A0      4
#define LCD_RW      5
#define LCD_E       6

#define KEYBOARD_SCAN_LINES     4
#define KEYBOARD_FEEDBACK_LINES 5
#define KEYBOARD_DELAY_US       10



void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursorPosition(uint8_t x, uint8_t y);
void LCD_PutString(const char *data);
void LCD_InsertChars(const char *data, uint8_t count);
void LCD_PutStringXY(uint8_t x, uint8_t y, const char *data);
void LCD_InsertCharsXY(uint8_t x, uint8_t y, const char *data, uint8_t count);
void LCD_CaptureKeyboard(void);


