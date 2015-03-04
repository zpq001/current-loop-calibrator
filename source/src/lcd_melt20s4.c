

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "lcd_melt20s4.h"
#include "dwt_delay.h"

#define DB_INPUT    0
#define DB_OUTPUT   1

void set_db4(uint8_t byte) {
    LCD_PORT->RXTX = byte & 0x0F;
}

uint8_t get_db4(void) {
    return LCD_PORT->RXTX & 0x0F;
}
void set_db_dir(uint8_t db_direction) {
    if (db_direction == DB_OUTPUT)
        LCD_PORT->OE |= (1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7);
    else
        LCD_PORT->OE &= ~(1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7);
}

void set_ctrl(uint8_t line, uint8_t new_state) {
    if (new_state)
        LCD_PORT->RXTX |= (1<<line);
    else
        LCD_PORT->RXTX &= ~(1<<line);
}

void write_db4(uint8_t byte) {
    set_db4(byte);
    DWT_DelayUs(1);
    set_ctrl(LCD_E, 1);
    DWT_DelayUs(2);
    set_ctrl(LCD_E, 0);
    DWT_DelayUs(1);
}

uint8_t read_db4(void) {
    uint8_t temp8u;
    DWT_DelayUs(1);
    set_ctrl(LCD_E, 1);
    DWT_DelayUs(2);
    temp8u = get_db4();
    set_ctrl(LCD_E, 0);
    DWT_DelayUs(1);
    return temp8u;
}

void write_byte(uint8_t data, uint8_t type) {
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
    write_db4(temp8u >> 4);
    write_db4(temp8u);
}

void write_cmd(uint8_t cmd) {
    write_byte(cmd, 0);
}

void write_data(uint8_t data) {
    write_byte(data, 1);
}


void LCD_Init(void) {
    
    PORT_InitTypeDef PORT_InitStructure;
    
    // Setup GPIO
    PORT_StructInit(&PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (PORT_MODE_DIGITAL << LCD_A0) | (PORT_MODE_DIGITAL << LCD_RW) | (PORT_MODE_DIGITAL << LCD_E);
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_OE = PORT_OE_OUT;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(MDR_PORTA, &PORT_InitStructure);
    PORT_InitStructure.PORT_Pin = (PORT_MODE_DIGITAL << LCD_DB4) | (PORT_MODE_DIGITAL << LCD_DB5) | (PORT_MODE_DIGITAL << LCD_DB6) | (PORT_MODE_DIGITAL << LCD_DB7);
    PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
    PORT_InitStructure.PORT_OE = PORT_OE_IN;
    PORT_Init(MDR_PORTA, &PORT_InitStructure);
    
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
    write_db4(0x03);
    write_db4(0x03);
    write_db4(0x03);
    write_db4(0x02);    // <- 4-bit mode
    
    write_cmd(0x2A);
	write_cmd(0x0C);
	write_cmd(0x01);
	write_cmd(0x06);
    
    write_data('a');
    write_data('b');
    write_data('c');
    
}




