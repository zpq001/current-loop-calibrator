/**********************************************************
	Module for processing buttons and switches
	using bit masks
	
	Type: Header file
	Written by: AvegaWanderer 10.2013
**********************************************************/

#include <stdint.h>


// User definitions
#define KEY_OUTPUT_CTRL     (1<<19)
#define KEY_OK              (1<<18)
#define KEY_NUM3            (1<<17)
#define KEY_NUM2            (1<<16)
#define KEY_NUM1            (1<<15)

#define KEY_ESC             (1<<13)
#define KEY_NUM6            (1<<12)
#define KEY_NUM5            (1<<11)
#define KEY_NUM4            (1<<10)

#define KEY_OUTPUT_CONST    (1<<8)
#define KEY_NUM9            (1<<7)
#define KEY_NUM8            (1<<6)
#define KEY_NUM7            (1<<5)

#define KEY_OUTPUT_WAVE   (1<<3)
#define KEY_BACKSPACE       (1<<2)
#define KEY_DOT             (1<<1)
#define KEY_NUM0            (1<<0)

#define ANY_KEY             ~(0x00);


//=============================================//
// Button processing setup

// Select minimal data type which can hold all your buttons (one bit per button or switch)
#define btn_type_t		uint32_t		

// Choose which actions are supported by button processor - comment unused to optimize code size
// ACTION_UP and ACTION_TOGGLE do not require timer

#define USE_ACTION_REP				// Emulation of repeated pressing
#define USE_ACTION_UP					// Triggers when button is released
#define USE_ACTION_UP_SHORT			// Triggers when button is released and had been pressed for short time
#define USE_ACTION_UP_LONG			// Triggers when button is released and had been pressed for long time
#define USE_ACTION_HOLD				// Triggers when button is being pressed for long time
#define USE_ACTION_TOGGLE				// Toggles every time button is pressed


// Set time options - all delays are in units of ProcessButtons() call period.
#define LONG_PRESS_DELAY	25		// After this delay button actions will be considered as long
#define REPEAT_DELAY		15		// After this delay emulation of repeated pressing becomes active

// Set inversion of raw_button_state
#define RAW_BUTTON_INVERSE_MASK		0x00
//=============================================//


// Extra defines used for cleaning unnecessary code - do not modify
#ifdef USE_ACTION_REP
	#define USE_BUTTON_TIMER
	#define USE_DELAYED
#endif
#ifdef USE_ACTION_UP
	#define USE_DELAYED
	#define USE_CURRENT_INVERSED	
#endif
#ifdef USE_ACTION_UP_SHORT
	#define USE_BUTTON_TIMER
	#define USE_DELAYED
	#define USE_CURRENT_INVERSED
#endif
#ifdef USE_ACTION_UP_LONG
	#define USE_BUTTON_TIMER
	#define USE_DELAYED
	#define USE_CURRENT_INVERSED
#endif
#ifdef USE_ACTION_HOLD
	#define USE_BUTTON_TIMER
	#define USE_DELAYED
#endif

#define _FF	(~0x00)

// Button state structure
typedef struct {
	btn_type_t raw_state;		// Non-processed button state (only RAW_BUTTON_INVERSE_MASK is applied)
	btn_type_t action_down;		// Bit is set once when button is pressed
	#ifdef USE_ACTION_REP
	btn_type_t action_rep;		// Bit is set once when button is pressed. Bit is set continuously after REPEAT_DELAY until button is released
	#endif
	#ifdef USE_ACTION_UP
	btn_type_t action_up;		// Bit is set once when button is released
	#endif
	#ifdef USE_ACTION_UP_SHORT
	btn_type_t action_up_short;	// Bit is set once when button is released before LONG_PRESS_DELAY
	#endif
	#ifdef USE_ACTION_UP_LONG
	btn_type_t action_up_long;	// Bit is set once when button is released after LONG_PRESS_DELAY
	#endif
	#ifdef USE_ACTION_HOLD
	btn_type_t action_hold;		// Bit is set once when button is pressed for LONG_PRESS_DELAY
	#endif
	#ifdef USE_ACTION_TOGGLE
	btn_type_t action_toggle;	// Bit is set every time button is pressed
	#endif
} buttons_t;



//=============================================//
// Processed output
extern buttons_t buttons;

//=============================================//
void InitButtons(void);
void ProcessButtons(void);

//=============================================//
// The value returned by GetRawButtonState() represents sampled state of buttons being processed - input data for ProcessButtons()
// It is assumed that if some button is pressed, corresponding bit is set.
// If your buttons are inversed, use RAW_BUTTON_INVERSE_MASK to invert bits.
// Only prototype is provided here. You should implement this functions somwhere in your code.
btn_type_t GetRawButtonState(void);


