
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*cbLcdUpdatePtr)(char **strings);


void registerLcdUpdateCallback(cbLcdUpdatePtr fptr);
void guiInitialize(void);
void guiButtonEvent(void);
void guiUpdate(void);



void LCD_Clear(void);
void LCD_SetCursorPosition(uint8_t x, uint8_t y);
void LCD_PutString(const char *data);
void LCD_InsertChars(const char *data, uint8_t count);
void LCD_PutStringXY(uint8_t x, uint8_t y, const char *data);
void LCD_InsertCharsXY(uint8_t x, uint8_t y, const char *data, uint8_t count);
/*
int32_t ExtADC_GetCurrent(void);
uint8_t ExtADC_GetRange(void);

uint32_t ADC_GetLoopCurrent(void);
uint8_t ADC_GetLoopStatus(void);
*/





#ifdef __cplusplus
}
#endif
