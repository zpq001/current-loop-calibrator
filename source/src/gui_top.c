
#include <string.h>
#include "utils.h"
#include "buttons.h"
#include "gui_core.h"
#include "gui_top.h"
#include "adc.h"
#include "external_adc.h"

#ifndef _MENU_SIMULATOR_
#include "lcd_melt20s4.h"
#else
#include "gui_wrapper.h"
#endif







static void mfConstSource_Select(void);
static void mfConstSource_Run(void);
static void mfConstSource_Leave(void);

static void mfAlternSource_Select(void);
static void mfAlternSource_Run(void);
static void mfAlternSource_Leave(void);
    
    
const MenuJumpRecord_t menuJumpSet[] = 
{
//        Item       |       Jump condition      |     Next item      |    Flags/Timeout
    { mi_START,             KEY_OK,                 mi_CALIBRATION,         JUMP_IF_EXACT       },
    { mi_START,             0,                      mi_CONSTSOURCE,         JUMP_ALWAYS         },
    { mi_CONSTSOURCE, 	    KEY_OUTPUT_ALTERN,      mi_ALTERNSOURCE,        JUMP_IF_EXACT       },
    { mi_ALTERNSOURCE, 	    KEY_OUTPUT_CONST,       mi_CONSTSOURCE,         JUMP_IF_EXACT       },
    { 0,                    0,                      0,                      0                   }
};



const MenuFunctionRecord_t menuFunctionSet[] =
{
//        Item          |      select function      |     run function      |    leave funtion
    { mi_CONSTSOURCE,	    mfConstSource_Select, 	    mfConstSource_Run,      mfConstSource_Leave	    },
    { mi_ALTERNSOURCE,      mfAlternSource_Select,      mfAlternSource_Run,     mfAlternSource_Leave    },
    { 0,                    0,                          0,                      0                       }
};


static uint8_t selectedMenuItemId;
static const MenuFunctionRecord_t *selectedMenuFunctionRecord;



void GUI_Init(void) {
    selectedMenuItemId = mi_START;
    selectedMenuFunctionRecord = getMenuFunctionRecord(selectedMenuItemId);
}


void GUI_Process(void) {
    uint8_t nextItemId;
    uint32_t jumpCondition;
    
    jumpCondition = buttons.action_down;
    nextItemId = getNextMenuItem(selectedMenuItemId, jumpCondition);
    
    if (nextItemId != selectedMenuItemId) {
        processItemFunction(selectedMenuFunctionRecord, LEAVE_FUNCTION);
        selectedMenuItemId = nextItemId;
        selectedMenuFunctionRecord = getMenuFunctionRecord(selectedMenuItemId);
        processItemFunction(selectedMenuFunctionRecord, SELECT_FUNCTION);
    } else {
        processItemFunction(selectedMenuFunctionRecord, RUN_FUNCTION);
    }
}

//---------------------------------------------//
// mfConstSource

static void mfConstSource_Select(void) {
    // Draw static text
    LCD_Clear();
    LCD_PutStringXY(0,0,"Уст. ток:");
    LCD_PutStringXY(10,1,"Uвых=    В");
    LCD_PutStringXY(0,2,"Постоянный ток");
    LCD_PutStringXY(0,3,"Амперметр:        мА");
}

static void mfConstSource_Run(void) {
    int32_t temp32;
    uint32_t temp32u;
    char str[10];
    
    // Loop current setting
    
    // Loop health
    if (ADC_GetLoopStatus() == LOOP_OK) 
        LCD_PutStringXY(0,1,"Подключ.");
    else
        LCD_PutStringXY(0,1,"Обрыв   ");
    
    // Output voltage
    temp32 = ADC_GetLoopVoltage();
    i32toa_align_right(temp32, str, 5, 2, 1);
    LCD_InsertCharsXY(15, 1, &str[0], 4);
    
    // Ampermeter result
    temp32 = ExtADC_GetCurrent();
    i32toa_align_right(temp32, str, 10, 4, 3);
    switch (ExtADC_GetRange()) {
        case EXTADC_LOW_RANGE:
            LCD_InsertCharsXY(13, 3, &str[2], 5);
            break;
        case EXTADC_HIGH_RANGE:
            LCD_InsertCharsXY(13, 3, &str[0], 5);
            break;
        default:
            str[3] = str[4] = '-';  // Overload
            LCD_InsertCharsXY(13, 3, &str[0], 5);
    }
}

static void mfConstSource_Leave(void) {
    
}


//---------------------------------------------//
// mfAlternSource

static void mfAlternSource_Select(void) {
    // Draw static text
    LCD_Clear();

}

static void mfAlternSource_Run(void) {
    
}
    
static void mfAlternSource_Leave(void) {
    
}



