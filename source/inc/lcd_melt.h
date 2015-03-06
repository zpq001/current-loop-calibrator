/****************************************************************//*
	@brief Module lcd_melt
	
		Command definitions for text MELT LCDs
	
********************************************************************/

enum LCD_COMMANDS {
    CMD_CLEAR = 0x01,
    CMD_GO_INITIAL_STATE = 0x02,
    CMD_SET_DATA_INPUT_MODE = 0x40,
    CMD_SET_LCD_STATE = 0x08,
    CMD_SHIFT = 0x10,
    CMD_SET_FUNC = 0x20,
    CMD_SET_CGRAM_ADDRESS = 0x40,
    CMD_SET_DDRAM_ADDRESS = 0x80
};
    
// SET_DATA_INPUT_MODE
#define OPT_DEC_DDRAM_ADDR  (0<<1)
#define OPT_INC_DDRAM_ADDR  (1<<1)
#define OPT_NOT_SHIFT_LCD   (0<<0)
#define OPT_DO_SHIFT_LCD    (1<<0)

// SET_LCD_STATE
#define OPT_LCD_OFF         (0<<2)
#define OPT_LCD_ON          (1<<2)
#define OPT_CURSOR_OFF      (0<<1)
#define OPT_CURSOR_ON       (1<<1)
#define OPT_CURSOR_STEADY   (0<<0)
#define OPT_CURSOR_BLINK    (1<<0)

// SHIFT
#define OPT_SHIFT_CURSOR    (0<<3)
#define OPT_SHIFT_LCD       (1<<3)
#define OPT_SHIFT_RIGHT     (1<<2)
#define OPT_SHIFT_LEFT      (0<<2)

// SET_FUNC
#define OPT_4BIT_INTERFACE  (0<<4)
#define OPT_8BIT_INTERFACE  (1<<4)
#define OPT_ONE_ROW_LCD     (0<<3)
#define OPT_TWO_ROW_LCD     (1<<3)
#define OPT_FONT_5x8        (0<<2)
#define OPT_FONT_5x11       (1<<2)
#define OPT_CGROM_PAGE0     (0<<1)
#define OPT_CGROM_PAGE1     (1<<1)
#define OPT_NO_INVERSION    (0<<0)
#define OPT_INVERSION       (1<<0)



