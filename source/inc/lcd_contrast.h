

#define CPWM_PORT	MDR_PORTE
#define CPWM_PIN	3

void LCD_InitContrastBooster(void);
void LCD_ProcessContrastBooster(void);
uint8_t LCD_SetContrastSetting(int32_t value);
uint8_t LCD_GetContrastSetting(void);

