/****************************************************************//*
	@brief Module lcd_melt20s4
	
	Functions for MELT 20 x 4 LCD
    
	Matrix keyboard shares scan lines with DB7-DB4, so this module
    also contains key state capture function
    
********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "lcd_melt.h"
#include "lcd_melt20s4.h"
#include "dwt_delay.h"

//---------------------------------------------//
// Private definitions

#define DB_INPUT    0   // data bus GPIOs are input
#define DB_OUTPUT   1   // data bus GPIOs are output
#define BYTE_DATA   1   // Transmitted byte is data
#define BYTE_CMD    0   // Transmitted byte is command

typedef struct {
    MDR_PORT_TypeDef* port;
    uint8_t pin;
} keyboard_pp;

static keyboard_pp keyboard_fb_lines[KEYBOARD_FEEDBACK_LINES] = {
    {MDR_PORTA, 7},
    {MDR_PORTB, 10},
    {MDR_PORTB, 9},
    {MDR_PORTB, 8},
    {MDR_PORTB, 7}
};

static uint32_t raw_keyboard_state;
    

//---------------------------------------------//
// Private functions

static void set_db4(uint8_t byte) {
	uint32_t temp = LCD_PORT->RXTX;
	temp &= ~0x0F;
	temp |= (byte & 0x0F);
    LCD_PORT->RXTX = temp;
}

static uint8_t get_db4(void) {
    return LCD_PORT->RXTX & 0x0F;
}

static void set_db_dir(uint8_t db_direction) {
    if (db_direction == DB_OUTPUT)
        LCD_PORT->OE |= (1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7);
    else
        LCD_PORT->OE &= ~((1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7));
}

static void set_ctrl(uint8_t line, uint8_t new_state) {
    if (new_state)
        LCD_PORT->RXTX |= (1<<line);
    else
        LCD_PORT->RXTX &= ~(1<<line);
}

static void write_db4(uint8_t byte) {
    set_db4(byte);
    DWT_DelayUs(20);
    set_ctrl(LCD_E, 1);
    DWT_DelayUs(20);
    set_ctrl(LCD_E, 0);
    DWT_DelayUs(20);
}

static uint8_t read_db4(void) {
    uint8_t temp8u;
    DWT_DelayUs(20);
    set_ctrl(LCD_E, 1);
    DWT_DelayUs(20);
    temp8u = get_db4();
    set_ctrl(LCD_E, 0);
    DWT_DelayUs(20);
    return temp8u;
}

static void write_byte(uint8_t type, uint8_t data) {
    uint8_t temp8u;
    // First wait for LCD to get ready
    set_db_dir(DB_INPUT);
    set_ctrl(LCD_RW, 1);
    set_ctrl(LCD_A0, 0);
    do {
        temp8u = read_db4();
        read_db4();
    } while(temp8u & (1<<3));
    set_db_dir(DB_OUTPUT);
    set_ctrl(LCD_RW, 0);
    set_ctrl(LCD_A0, type);
    write_db4(data >> 4);
    write_db4(data);
}


//---------------------------------------------//
// Global functions


void LCD_Init(void) {
    uint8_t i;
    
    PORT_InitTypeDef PORT_InitStructure;
    
    // Setup GPIO for LCD
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << LCD_A0) | (1 << LCD_RW) | (1 << LCD_E);
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_OE = PORT_OE_OUT;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(LCD_PORT, &PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7);
    PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
    PORT_InitStructure.PORT_OE = PORT_OE_IN;
    PORT_Init(LCD_PORT, &PORT_InitStructure);
    
    // Setup GPIO for keyboard feedback
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
    PORT_InitStructure.PORT_OE = PORT_OE_IN;
    for (i=0; i<KEYBOARD_FEEDBACK_LINES; i++) {
        PORT_InitStructure.PORT_Pin = (PORT_MODE_DIGITAL << keyboard_fb_lines[i].pin);
        PORT_Init(keyboard_fb_lines[i].port, &PORT_InitStructure);
    }
    
    /*
    LCD_PORT->ANALOG = (PORT_MODE_DIGITAL << LCD_DB4) | (PORT_MODE_DIGITAL << LCD_DB5) | (PORT_MODE_DIGITAL << LCD_DB6) | (PORT_MODE_DIGITAL << LCD_DB7) |\
                       (PORT_MODE_DIGITAL << LCD_A0) | (PORT_MODE_DIGITAL << LCD_RW) | (PORT_MODE_DIGITAL << LCD_E);
    LCD_PORT->PULL = 0;     // no pull-up/down
    LCD_PORT->RXTX = 0;     // data io
    LCD_PORT->FUNC = 0;     // 00 - port, 01 - main, 10 - altern, 11 - redef
    LCD_PORT->PD = 0;       // [15:0]: 0-driver, 1-open drain. [31:16]: 0-normal input, 1 - Shimdt trigger 400mV
    LCD_PORT->PWR = (PORT_SPEED_FAST << LCD_DB4*2) | (PORT_SPEED_FAST << LCD_DB5*2) | (PORT_SPEED_FAST << LCD_DB6*2) | (PORT_SPEED_FAST << LCD_DB7*2) |\
                    (PORT_SPEED_FAST << LCD_A0*2) | (PORT_SPEED_FAST << LCD_RW*2) | (PORT_SPEED_FAST << LCD_E*2);
    LCD_PORT->GFEN = 0;     // input filter disable
    LCD_PORT->OE = (PORT_OE_OUT << LCD_DB4) | (PORT_OE_OUT << LCD_DB5) | (PORT_OE_OUT << LCD_DB6) | (PORT_OE_OUT << LCD_DB7) |\
                   (PORT_OE_OUT << LCD_A0) | (PORT_OE_OUT << LCD_RW) | (PORT_OE_OUT << LCD_E);
    */
    
    // Initialize sequence
    set_ctrl(LCD_E, 0);
    set_ctrl(LCD_RW, 0);
    set_ctrl(LCD_A0, 0);
    DWT_DelayUs(20000);
	set_db_dir(DB_OUTPUT);
    write_db4(0x03);
	DWT_DelayUs(50);
    write_db4(0x03);
	DWT_DelayUs(50);
    write_db4(0x03);
	DWT_DelayUs(50);
    write_db4(0x02);    // <- 4-bit mode
	DWT_DelayUs(50);
    
    // Setup LCD
    write_byte(BYTE_CMD, CMD_SET_FUNC | OPT_4BIT_INTERFACE | OPT_TWO_ROW_LCD | OPT_FONT_5x8 | OPT_CGROM_PAGE1 | OPT_NO_INVERSION);
    write_byte(BYTE_CMD, CMD_SET_LCD_STATE | OPT_LCD_ON | OPT_CURSOR_OFF | OPT_CURSOR_STEADY);
    //write_byte(BYTE_CMD, CMD_SET_LCD_STATE | OPT_LCD_OFF | OPT_CURSOR_OFF | OPT_CURSOR_STEADY);
    write_byte(BYTE_CMD, CMD_CLEAR);
    write_byte(BYTE_CMD, CMD_SET_DATA_INPUT_MODE | OPT_INC_DDRAM_ADDR | OPT_NOT_SHIFT_LCD);
    
	//write_byte(BYTE_CMD, CMD_SET_LCD_STATE | OPT_LCD_ON | OPT_CURSOR_OFF | OPT_CURSOR_STEADY);
	
    LCD_SetCursorPosition(0,0); // for sure
   
    //LCD_PutString("Калибратор токовой");
    //LCD_SetCursorPosition(0,1); 
    //LCD_PutString("  петли 0-20мА");
    //LCD_SetCursorPosition(0,3); 
    //LCD_PutString("Версия 0.1");
    
    LCD_PutString("Current loop");
    LCD_SetCursorPosition(0,1); 
    LCD_PutString("calibrator 0-20мА");
    LCD_SetCursorPosition(0,2); 
    LCD_PutString("Привет!");
    LCD_SetCursorPosition(0,3); 
    LCD_PutString("Version 0.1");
}

void LCD_Clear(void) {
    write_byte(BYTE_CMD, CMD_CLEAR);    // CHECKME
}

void LCD_SetCursorPosition(uint8_t x, uint8_t y) {
    if (x > 19) x = 0;
    switch (y) {
        case 1:
            x += 20;    break;
        case 2:
            x += 64;    break;
        case 3:
            x += 64+20; break;
    }
    write_byte(BYTE_CMD, CMD_SET_DDRAM_ADDRESS | x);
}

void LCD_PutString(const char *data) {
    while (*data) {
        write_byte(BYTE_DATA, *data++);
    }
}

void LCD_InsertChars(const char *data, uint8_t count) {
    while (count--) {
        write_byte(BYTE_DATA, *data++);
    }
}

void LCD_PutStringXY(uint8_t x, uint8_t y, const char *data) {
    LCD_SetCursorPosition(x, y);
    LCD_PutString(data);
}

void LCD_InsertCharsXY(uint8_t x, uint8_t y, const char *data, uint8_t count) {
    LCD_SetCursorPosition(x, y);
    LCD_InsertChars(data, count);
}

void LCD_CaptureKeyboard(void) {
    uint8_t i,j;
    uint32_t keyboard_feedback;
    raw_keyboard_state = 0;
    for (i=0; i<KEYBOARD_SCAN_LINES; i++) {
        set_db4(0x01 << i);
        DWT_DelayUs(KEYBOARD_DELAY_US);
        keyboard_feedback = 0;
        for (j=0; j<KEYBOARD_FEEDBACK_LINES; j++) {
            keyboard_feedback |= (keyboard_fb_lines[j].port->RXTX & (1<<keyboard_fb_lines[j].pin)) ? (1<<j) : 0;
        }
        raw_keyboard_state <<= KEYBOARD_FEEDBACK_LINES;
        raw_keyboard_state |= keyboard_feedback;
    }
    set_db4(0x00);
}

uint32_t GetRawButtonState(void) {
    return raw_keyboard_state;
}



