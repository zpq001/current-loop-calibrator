
#include <string.h>
#include "utils.h"
#include "buttons.h"
#include "gui_core.h"
#include "gui_top.h"
#include "adc.h"
#include "external_adc.h"
#include "dac.h"
#include "encoder.h"

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

static void mfWaveEdit_Select(void);
static void mfWaveEdit_Run(void);
static void mfWaveEdit_Leave(void);


const MenuJumpRecord_t menuJumpSet[] = 
{
//        Item       |       Jump condition      |     Next item      |    Flags
    { mi_START,             KEY_OK,                 mi_CALIBRATION,         JUMP_IF_EXACT   | KEY_ACT_DOWN  },
    { mi_START,             0,                      mi_CONSTSOURCE,         JUMP_ALWAYS                     },
    { mi_CONSTSOURCE, 	    KEY_OUTPUT_ALTERN,      mi_ALTERNSOURCE,        JUMP_IF_EXACT   | KEY_ACT_DOWN  },
    { mi_ALTERNSOURCE, 	    KEY_OUTPUT_CONST,       mi_CONSTSOURCE,         JUMP_IF_EXACT   | KEY_ACT_DOWN  },
    { mi_ALTERNSOURCE, 	    KEY_NUM7,               mi_WAVEAMPLOWEDIT,      JUMP_IF_EXACT   | KEY_ACT_HOLD  },
    { mi_ALTERNSOURCE, 	    KEY_NUM8,               mi_WAVEAMPHIGHEDIT,     JUMP_IF_EXACT   | KEY_ACT_HOLD  },

    //{ mi_WAVEAMPLOWEDIT,    KEY_OK,                 mi_ALTERNSOURCE,        JUMP_IF_EXACT   | KEY_ACT_DOWN  },
    //{ mi_WAVEAMPLOWEDIT,    KEY_ESC,                mi_ALTERNSOURCE,        JUMP_IF_EXACT   | KEY_ACT_DOWN  },
    { mi_WAVEAMPLOWEDIT,    KEY_OUTPUT_ALTERN,      mi_ALTERNSOURCE,        JUMP_IF_EXACT   | KEY_ACT_DOWN  },
    { mi_WAVEAMPLOWEDIT,    KEY_OUTPUT_CONST,       mi_CONSTSOURCE,         JUMP_IF_EXACT   | KEY_ACT_DOWN  },
    { mi_WAVEAMPHIGHEDIT,   KEY_OUTPUT_ALTERN,      mi_ALTERNSOURCE,        JUMP_IF_EXACT   | KEY_ACT_DOWN  },
    { mi_WAVEAMPHIGHEDIT,   KEY_OUTPUT_CONST,       mi_CONSTSOURCE,         JUMP_IF_EXACT   | KEY_ACT_DOWN  },

    { 0,                    0,                      0,                      0                   }
};



const MenuFunctionRecord_t menuFunctionSet[] =
{
//        Item          |      select function      |     run function      |    leave funtion
    { mi_CONSTSOURCE,	    mfConstSource_Select, 	    mfConstSource_Run,      mfConstSource_Leave	    },
    { mi_ALTERNSOURCE,      mfAlternSource_Select,      mfAlternSource_Run,     mfAlternSource_Leave    },
    { mi_WAVEAMPLOWEDIT,    mfWaveEdit_Select,          mfWaveEdit_Run,         mfWaveEdit_Leave        },
    { mi_WAVEAMPHIGHEDIT,   mfWaveEdit_Select,          mfWaveEdit_Run,         mfWaveEdit_Leave        },
    { 0,                    0,                          0,                      0                       }
};


static uint8_t selectedMenuItemId;
static const MenuFunctionRecord_t *selectedMenuFunctionRecord;

static uint8_t editMode;
edit_t edit;
uint8_t editorCode;
uint8_t editorCodeValid;



static void encodeEditorKeys(void) {
    editorCodeValid = 1;
    if (buttons.action_up_short & KEY_NUM0) editorCode = EDIT_NUM0;
    else if (buttons.action_up_short & KEY_NUM1) editorCode = EDIT_NUM1;
    else if (buttons.action_up_short & KEY_NUM2) editorCode = EDIT_NUM2;
    else if (buttons.action_up_short & KEY_NUM3) editorCode = EDIT_NUM3;
    else if (buttons.action_up_short & KEY_NUM4) editorCode = EDIT_NUM4;
    else if (buttons.action_up_short & KEY_NUM5) editorCode = EDIT_NUM5;
    else if (buttons.action_up_short & KEY_NUM6) editorCode = EDIT_NUM6;
    else if (buttons.action_up_short & KEY_NUM7) editorCode = EDIT_NUM7;
    else if (buttons.action_up_short & KEY_NUM8) editorCode = EDIT_NUM8;
    else if (buttons.action_up_short & KEY_NUM9) editorCode = EDIT_NUM9;
    else if (buttons.action_up_short & KEY_DOT) editorCode = EDIT_DOT;
    else if (buttons.action_up_short & KEY_BACKSPACE) editorCode = EDIT_BKSPACE;
    else editorCodeValid = 0;
}



void GUI_Init(void) {
    selectedMenuItemId = mi_START;
    selectedMenuFunctionRecord = getMenuFunctionRecord(selectedMenuItemId);
    editMode = 0;
}


void GUI_Process(void) {
    uint8_t nextItemId;
    nextItemId = getNextMenuItem(selectedMenuItemId);
    
    //if ((selectedMenuItemId == mi_ALTERNSOURCE) && (nextItemId == mi_WAVEAMPLOWEDIT)) {
    //	nextItemId = getNextMenuItem(selectedMenuItemId);
    //}
	
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
    LCD_PutStringXY(0,0,"Уст. ток:         мА");
    LCD_PutStringXY(10,1,"Uвых=    В");
    LCD_PutStringXY(0,2,"Пост. ток. Профиль х");
    LCD_PutStringXY(0,3,"Амперметр:        мА");

    DAC_SetMode(DAC_MODE_CONST);
}

static void mfConstSource_Run(void) {
    int32_t temp32;
    uint32_t temp32u;
    uint8_t temp8u;
    char str[10];
    
    // Loop current setting

    // Check the keys
    encodeEditorKeys();

    // Control
    if (!editMode) {
        // Capture numeric keys
        if (editorCodeValid) {
            startEditor(&edit, editorCode, 1);
            editMode = 1;
        }
    } else {
        // Check enter or cancel keys
        if (buttons.action_down & KEY_OK) {
            temp32u = getScaledEditValue(&edit, 3);
            DAC_SetSettingConst(temp32u);
            editMode = 0;
        } else if (buttons.action_down & KEY_ESC) {
            editMode = 0;
        } else if (editorCodeValid)
            processEditor(&edit, editorCode);
    }
	
    // Temporary!!!!
    if (buttons.action_hold & KEY_NUM2) {
        DAC_SaveSettings();
		__disable_irq();
		EE_SaveSettings();
		__enable_irq();
	}
    else if (buttons.action_hold & KEY_NUM1) {
		__disable_irq();
		EE_RestoreSettings();
		__enable_irq();
        DAC_RestoreSettings();
	}
	

    // Display
    if (!editMode) {
        temp32u = DAC_GetSettingConst();
        i32toa_align_right(temp32u, str, 10, 4, 3);
        LCD_InsertCharsXY(13, 0, &str[2], 5);
    } else {
        temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
        i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
        LCD_InsertCharsXY(14, 0, &str[5], 4);
    }


    // Loop health
    if (ADC_GetLoopStatus() == LOOP_OK) 
        LCD_PutStringXY(0,1,"Подключ.");
    else
        LCD_PutStringXY(0,1,"Обрыв   ");
    
    // Output voltage
    temp32 = ADC_GetLoopVoltage();
    i32toa_align_right(temp32, str, 10, 4, 3);
    LCD_InsertCharsXY(15, 1, &str[3], 4);
    
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
    editMode = 0;
}


//---------------------------------------------//
// mfAlternSource

static void mfAlternSource_Select(void) {
    // Draw static text
    LCD_Clear();
    LCD_PutStringXY(0,0,"Сигнал:");
    LCD_PutStringXY(0,1,"Период:            с");
    LCD_PutStringXY(0,2,"Циклов:       /     ");
    LCD_PutStringXY(0,3,"Амперметр:        мА");

    DAC_SetMode(DAC_MODE_WAVEFORM);
}

static void mfAlternSource_Run(void) {
    int32_t temp32;
    uint32_t temp32u;
    uint8_t temp8u;
    char str[10];

    // Control waveform
    if (buttons.action_hold & KEY_NUM1)
        DAC_SetWaveform(WAVE_MEANDR);
    else if (buttons.action_hold & KEY_NUM2)
        DAC_SetWaveform(WAVE_SAW_DIRECT);
    else if (buttons.action_hold & KEY_NUM3)
        DAC_SetWaveform(WAVE_SAW_REVERSED);

    // Display waveform
    switch (DAC_GetWaveform()) {
        case WAVE_MEANDR:       LCD_PutStringXY(10,0,"    меандр"); break;
        case WAVE_SAW_DIRECT:   LCD_PutStringXY(10,0,"      пила"); break;
        case WAVE_SAW_REVERSED: LCD_PutStringXY(10,0," обр. пила"); break;
    }

    // Control waveform period
    if (encoder_delta != 0) {
        temp32 = DAC_GetPeriod();
        temp32 += encoder_delta * 100;
        if (temp32 < 100) temp32 = 100;
        else if (temp32 > 100000) temp32 = 1000;
        DAC_SetPeriod(temp32);
    }

    // Display waveform period
    temp32u = DAC_GetPeriod();
    i32toa_align_right(temp32u, str, 10, 4, 3);
    LCD_InsertCharsXY(14, 1, &str[2], 5);

    // Control cycles

    // Check the keys
    encodeEditorKeys();

    // Control
    if (!editMode) {
        // Capture numeric keys
        if (editorCodeValid) {
            startEditor(&edit, editorCode, 1);
            editMode = 1;
        }
    } else {
        // Check enter or cancel keys
        if (buttons.action_down & KEY_OK) {
            temp32u = getScaledEditValue(&edit, 0);
            DAC_SetTotalCycles(temp32u);
            editMode = 0;
        } else if (buttons.action_down & KEY_ESC) {
            editMode = 0;
        } else if (editorCodeValid)
            processEditor(&edit, editorCode);
    }

    // Display cycles
    temp32u = DAC_GetCurrentCycle();
    i32toa_align_right(temp32u, str, 10, 1, -1);
    LCD_InsertCharsXY(9, 2, &str[4], 5);

    if (!editMode) {
        temp32u = DAC_GetTotalCycles();
        i32toa_align_right(temp32u, str, 10, 1, -1);
        LCD_InsertCharsXY(15, 2, &str[4], 5);
    } else {
        temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
        i32toa_align_right(edit.value , str, 10, temp8u, -1);
        LCD_InsertCharsXY(15, 2, &str[4], 5);
    }

    // Check restart button
    if (buttons.action_down & KEY_OUTPUT_CTRL)
        DAC_RestartCycles();



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
    
static void mfAlternSource_Leave(void) {

}


static void mfWaveEdit_Select(void) {
    // Draw static text
    LCD_Clear();
    LCD_PutStringXY(0,0,"Амплитуда сигнала:");
    //LCD_PutStringXY(0,2,"Уст. нижн.        мА");
    //LCD_PutStringXY(0,3,"Уст. верхн.       мА");
    if (selectedMenuItemId == mi_WAVEAMPLOWEDIT)
        LCD_PutStringXY(0,2,"Минимум:          мА");
    else
        LCD_PutStringXY(0,2,"Максимум:         мА");
}

static void mfWaveEdit_Run(void) {
    uint32_t temp32u;
    char str[10];
    uint8_t temp8u;

    // Check the keys
    encodeEditorKeys();

    // Control
    if (!editMode) {
        // Capture numeric keys
        if (editorCodeValid) {
            startEditor(&edit, editorCode, 1);
            editMode = 1;
        }
    } else {
        // Check enter or cancel keys
        if (buttons.action_down & KEY_OK) {
            temp32u = getScaledEditValue(&edit, 3);
            if (selectedMenuItemId == mi_WAVEAMPLOWEDIT)
                DAC_SetSettingWaveMin(temp32u);
            else
                DAC_SetSettingWaveMax(temp32u);

            editMode = 0;
        } else if (buttons.action_down & KEY_ESC) {
            editMode = 0;
        } else if (editorCodeValid)
            processEditor(&edit, editorCode);
    }

    // Display
    if (!editMode) {
        if (selectedMenuItemId == mi_WAVEAMPLOWEDIT)
            temp32u = DAC_GetSettingWaveMin();
        else
            temp32u = DAC_GetSettingWaveMax();

        i32toa_align_right(temp32u, str, 10, 4, 3);
        LCD_InsertCharsXY(13, 2, &str[2], 5);
    } else {
        temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
        i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
        LCD_InsertCharsXY(14, 2, &str[5], 4);
    }
}

static void mfWaveEdit_Leave(void) {
    editMode = 0;
}


