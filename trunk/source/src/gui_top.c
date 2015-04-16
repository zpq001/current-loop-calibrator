/****************************************************************//*
	@brief Module GUI
	
	Implementation of user interface
	Module incorporates displaying device status and controlling device.
    
    
********************************************************************/

#include <string.h>
#include "utils.h"
#include "buttons.h"
#include "gui_core.h"
#include "gui_top.h"
#include "adc.h"
#include "external_adc.h"
#include "dac.h"
#include "encoder.h"
#include "lcd_contrast.h"
#include "power_monitor.h"
#include "sound.h"
#include "utils.h"

#ifndef _MENU_SIMULATOR_
#include "lcd_melt20s4.h"
#else
#include "gui_wrapper.h"
#endif

enum MenuItems{
    MENU_CONST_OUTPUT,
    MENU_WAVEFORM_OUTPUT,
    MENU_WAVEFORM_EDIT
};

enum SysMenuItems{
    SYS_CONTRAST, SYS_BEEPER,
    SYS_DAC_CALIBRATION_LOW,
    SYS_DAC_CALIBRATION_HIGH,
    SYS_ADC_VOLTAGE_CALIBRATION_LOW,
    SYS_ADC_VOLTAGE_CALIBRATION_HIGH,
    SYS_EXTADC_CALIBRATION_ZERO,
    SYS_EXTADC_CALIBRATION_LOW_RANGE,
    SYS_EXTADC_CALIBRATION_HIGH_RANGE,
    SYS_DONE
};

enum WaveEditModes {EDIT_LOW_AMP, EDIT_HIGH_AMP};

typedef void (*RunFuncPtr)(void);

static uint8_t menu_state;
static uint8_t first_visit;
static uint8_t wave_edit_mode;

static uint8_t editMode;
edit_t edit;
uint8_t editorCode;
uint8_t editorCodeValid;
uint8_t sound_event;
static uint8_t dac_cycles_done;
static uint8_t blink_counter;
static uint8_t blink_flag;
static const char spaces[] = "          ";

RunFuncPtr runFunction;

static void runCalibrationMode(void);
static void runNormalMode(void);


static void encodeEditorKeys(void) {
    editorCodeValid = 1;
    if (buttons.action_down & KEY_NUM0) editorCode = EDIT_NUM0;
    else if (buttons.action_down & KEY_NUM1) editorCode = EDIT_NUM1;
    else if (buttons.action_down & KEY_NUM2) editorCode = EDIT_NUM2;
    else if (buttons.action_down & KEY_NUM3) editorCode = EDIT_NUM3;
    else if (buttons.action_down & KEY_NUM4) editorCode = EDIT_NUM4;
    else if (buttons.action_down & KEY_NUM5) editorCode = EDIT_NUM5;
    else if (buttons.action_down & KEY_NUM6) editorCode = EDIT_NUM6;
    else if (buttons.action_down & KEY_NUM7) editorCode = EDIT_NUM7;
    else if (buttons.action_down & KEY_NUM8) editorCode = EDIT_NUM8;
    else if (buttons.action_down & KEY_NUM9) editorCode = EDIT_NUM9;
    else if (buttons.action_down & KEY_DOT) editorCode = EDIT_DOT;
    else if (buttons.action_down & KEY_BACKSPACE) editorCode = EDIT_BKSPACE;
    else editorCodeValid = 0;
}



void GUI_Init(void) {
    if (device_mode == MODE_NORMAL) {
        runFunction = runNormalMode;
        menu_state = MENU_CONST_OUTPUT;
    }
    else {
        runFunction = runCalibrationMode;
        menu_state = SYS_CONTRAST;
    }
    first_visit = 1;
    editMode = 0;
	dac_cycles_done = 0;
	blink_counter = 0;
}


void GUI_Process(void) {
	uint8_t edit_mode_old = editMode;
	blink_counter = (blink_counter < 20) ? blink_counter + 1 : 0;
	blink_flag = (blink_counter < 12) ? 1 : 0;
	
    sound_event = 0;
	if (dac_cycles_done) {
		dac_cycles_done = 0;
		sound_event = SE_CyclesDone;
	}
    runFunction();
    if (sound_event) {
        Sound_Event(sound_event);
    }
	
	if (edit_mode_old != editMode) {
		blink_counter = 0;
	}	
}

void DAC_OnCyclesDone(void) {
	dac_cycles_done = 1;
}


static void displayAmpMeter(void) {
    int32_t temp32;
    char str[10];
	uint8_t range = ExtADC_GetRange();

    // Ampermeter result
    temp32 = ExtADC_GetCurrent();

	if (range == EXTADC_LOW_RANGE) {    
		round_int32(&temp32, 1);
		i32toa_align_right(temp32, str, 10, 4, 3);
		LCD_InsertCharsXY(13, 3, &str[2], 5);
    } else {
		round_int32(&temp32, 2);
		i32toa_align_right(temp32, str, 10, 4, 3);
		if (range == EXTADC_HIGH_OVERLOAD)
			str[3] = str[4] = '*';  // Overload
        LCD_InsertCharsXY(13, 3, &str[0], 5);
	}
}


static void runNormalMode(void) {
    int32_t temp32;
    uint32_t temp32u;
    uint8_t temp8u;
    uint8_t new_state = menu_state;
    char str[10];

    encodeEditorKeys();
    switch (menu_state) {

        //---------------------------------------------//
        // Constant output
        case MENU_CONST_OUTPUT:
            if (first_visit) {
                LCD_Clear();
                LCD_PutStringXY(0,0,"Пост. ток. Профиль  ");
                LCD_PutStringXY(0,1,"Уст. ток:         мА");
                LCD_PutStringXY(0,2,"Выход:     B      мА");
                LCD_PutStringXY(0,3,"Амперметр:        мА");
                DAC_SetMode(DAC_MODE_CONST);
            }

            // Output control
            if (!editMode) {
                if (buttons.action_down & KEY_OK)
                    DAC_SetOutputState(1);
                else if (buttons.action_down & KEY_ESC)
                    DAC_SetOutputState(0);
            }

            // Profile number
            if (buttons.action_down & KEY_OUTPUT_CTRL) {
                temp32 = DAC_GetActiveProfile() + 1;
                temp8u = DAC_SetProfile((temp32 > DAC_PROFILE_COUNT) ? 1 : temp32);
                sound_event = (temp8u == VALUE_IN_RANGE) ? SE_SettingConfirm : SE_SettingIllegal;
                editMode = 0;
            }
            temp32 = DAC_GetActiveProfile();
            i32toa_align_right(temp32, str, 10, 1, -1);
            LCD_InsertCharsXY(19, 0, &str[8], 1);

            // Current setting control
			if (encoder_delta) {
				temp32 = DAC_GetSettingConst();
				temp32 += encoder_delta * 100;
				temp8u = DAC_SetSettingConst(temp32);
				sound_event = (temp8u == VALUE_IN_RANGE) ? SE_EncoderConfirm : SE_EncoderIllegal;
			}
            if (!editMode) {
                // Capture numeric keys
                if (editorCodeValid) {
                    startEditor(&edit, editorCode, 1, DAC_MAX_SETTING / 100);
                    editMode = 1;
                    sound_event = SE_KeyConfirm;
                }
            } else {
                // Check enter or cancel keys
                if (buttons.action_down & KEY_OK) {
                    temp32u = getScaledEditValue(&edit) * 100;
                    temp8u = DAC_SetSettingConst(temp32u);
                    sound_event = (temp8u == VALUE_IN_RANGE) ? SE_SettingConfirm : SE_SettingIllegal;
                    editMode = 0;
                } else if (buttons.action_down & KEY_ESC) {
                    editMode = 0;
                    sound_event = SE_KeyConfirm;
                } else if (editorCodeValid) {
                    temp8u = processEditor(&edit, editorCode);
                    sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
                }
            }
            // Current setting display
            if (!editMode) {
                temp32u = DAC_GetSettingConst();
                i32toa_align_right(temp32u, str, 10, 4, 3);
                LCD_InsertCharsXY(13, 1, &str[2], 5);
            } else {
                temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
				i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
				if (blink_flag)
					LCD_InsertCharsXY(14, 1, &str[5], 4);
				else 
					LCD_InsertCharsXY(14, 1, spaces, 4);
            }

            // Output current
            temp32u = ADC_GetLoopCurrent();
            round_uint32(&temp32u, 1);
            i32toa_align_right(temp32u, str, 10, 4, 3);
            LCD_InsertCharsXY(14, 2, &str[3], 4);

            // Output voltage
            temp32u = ADC_GetLoopVoltage();
            round_uint32(&temp32u, 1);
            i32toa_align_right(temp32u, str, 10, 4, 3);
            LCD_InsertCharsXY(7, 2, &str[3], 4);

            // Ampermeter result
            displayAmpMeter();

            // Next item
            if (buttons.action_down & KEY_OUTPUT_WAVE) {
                new_state = MENU_WAVEFORM_OUTPUT;
                sound_event = SE_KeyConfirm;
            }
            break;

        //---------------------------------------------//
        // Waveform output
        case MENU_WAVEFORM_OUTPUT:
            if (first_visit) {
                LCD_Clear();
                LCD_PutStringXY(0,0,"Сигнал:");
                LCD_PutStringXY(0,1,"Период:            с");
                LCD_PutStringXY(0,2,"Циклов:       /     ");
                LCD_PutStringXY(0,3,"Амперметр:        мА");
                DAC_SetMode(DAC_MODE_WAVEFORM);
            }

            // Output control
            if (!editMode) {
                if (buttons.action_down & KEY_OK)
                    DAC_SetOutputState(1);
                else if (buttons.action_down & KEY_ESC)
                    DAC_SetOutputState(0);
            }

            // Control waveform
            if (buttons.raw_state & KEY_OUTPUT_WAVE) {
                if (buttons.action_down & KEY_NUM1) {
                    DAC_SetWaveform(WAVE_MEANDR);
                    sound_event = SE_SettingConfirm;
                }
                else if (buttons.action_down & KEY_NUM2) {
                    DAC_SetWaveform(WAVE_SAW_DIRECT);
                    sound_event = SE_SettingConfirm;
                }
                else if (buttons.action_down & KEY_NUM3) {
                    DAC_SetWaveform(WAVE_SAW_REVERSED);
                    sound_event = SE_SettingConfirm;
                }
				editorCodeValid = 0;
            }
            // Display waveform
            switch (DAC_GetWaveform()) {
                case WAVE_MEANDR:       LCD_PutStringXY(10,0,"    меандр"); break;
                case WAVE_SAW_DIRECT:   LCD_PutStringXY(10,0,"      пила"); break;
                case WAVE_SAW_REVERSED: LCD_PutStringXY(10,0," обр. пила"); break;
            }
            // Control waveform period
            if (encoder_delta != 0) {
                temp32 = DAC_GetPeriod();
				if (temp32 >= 100000) {
					temp32 += encoder_delta * 1000;	// [1s]
					// Remove 100ms gradation if greater than 100s
					if (temp32 > 100000) {
						temp32 -= temp32 % 1000;
					}
				} else {
					temp32 += encoder_delta * 100;	// [100ms]
				}
                temp8u = DAC_SetPeriod(temp32);
                sound_event = (temp8u == VALUE_IN_RANGE) ? SE_EncoderConfirm : SE_EncoderIllegal;
            }
            // Display waveform period
            temp32u = DAC_GetPeriod();
            i32toa_align_right(temp32u, str, 10, 4, 3);
            LCD_InsertCharsXY(14, 1, &str[2], 5);

            // Check restart button
            if (buttons.action_down & KEY_OUTPUT_CTRL) {
                DAC_RestartCycles();
                sound_event = SE_KeyConfirm;
            }

            // Control cycles
            if (!editMode) {
                // Capture numeric keys
                if (editorCodeValid) {
                    //startEditor(&edit, editorCode, 1);
                    startEditor(&edit, editorCode, 0, DAC_CYCLES_MAX);
                    editMode = 1;
                    sound_event = SE_KeyConfirm;
                }
            } else {
                // Check enter or cancel keys
                if (buttons.action_down & KEY_OK) {
                    temp32u = getScaledEditValue(&edit);
                    temp8u = DAC_SetTotalCycles(temp32u);
                    sound_event = (temp8u == VALUE_IN_RANGE) ? SE_SettingConfirm : SE_SettingIllegal;
                    editMode = 0;
                } else if (buttons.action_down & KEY_ESC) {
                    editMode = 0;
                    sound_event = SE_KeyConfirm;
                } else if (editorCodeValid) {
                    temp8u = processEditor(&edit, editorCode);
                    sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
				}
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
                if (blink_flag)
					LCD_InsertCharsXY(15, 2, &str[4], 5);
				else 
					LCD_InsertCharsXY(15, 2, spaces, 5);
            }

            // Ampermeter result
            displayAmpMeter();

            // Next item
            if (buttons.action_down & KEY_OUTPUT_CONST) {
                new_state = MENU_CONST_OUTPUT;
                sound_event = SE_KeyConfirm;
            } else if (buttons.raw_state & KEY_OUTPUT_WAVE) {
                if (buttons.action_down & KEY_NUM7) {
                    new_state = MENU_WAVEFORM_EDIT;
                    wave_edit_mode = EDIT_LOW_AMP;
                    sound_event = SE_KeyConfirm;
                } else if (buttons.action_down & KEY_NUM8) {
                    new_state = MENU_WAVEFORM_EDIT;
                    wave_edit_mode = EDIT_HIGH_AMP;
                    sound_event = SE_KeyConfirm;
                }
            }
            break;

        //---------------------------------------------//
        // Waveform amplitude edit
        case MENU_WAVEFORM_EDIT:
            if (first_visit) {
                LCD_Clear();
                LCD_PutStringXY(0,0,"Амплитуда сигнала:");
                if (wave_edit_mode == EDIT_LOW_AMP)
                    LCD_PutStringXY(0,2,"Минимум:          мА");
                else
                    LCD_PutStringXY(0,2,"Максимум:         мА");
            }
            // Control
			if (encoder_delta) {
				if (wave_edit_mode == EDIT_LOW_AMP)
                    temp32 = DAC_GetSettingWaveMin();
                else
                    temp32 = DAC_GetSettingWaveMax();
				temp32 += encoder_delta * 100;
				if (wave_edit_mode == EDIT_LOW_AMP)
					temp8u = DAC_SetSettingWaveMin(temp32);
				else
					temp8u = DAC_SetSettingWaveMax(temp32);
				sound_event = (temp8u == VALUE_IN_RANGE) ? SE_EncoderConfirm : SE_EncoderIllegal;
			}
            if (!editMode) {
                if (editorCodeValid) {
                    startEditor(&edit, editorCode, 1, DAC_MAX_SETTING / 100);
                    editMode = 1;
                    sound_event = SE_KeyConfirm;
                }
            } else {
                // Check enter or cancel keys
                if (buttons.action_down & KEY_OK) {
                    temp32u = getScaledEditValue(&edit) * 100;
                    if (wave_edit_mode == EDIT_LOW_AMP)
                        temp8u = DAC_SetSettingWaveMin(temp32u);
                    else
                        temp8u = DAC_SetSettingWaveMax(temp32u);
                    sound_event = (temp8u == VALUE_IN_RANGE) ? SE_SettingConfirm : SE_SettingIllegal;
                    editMode = 0;
					buttons.action_down &= ~KEY_OK;		// prevent falling out
                } else if (buttons.action_down & KEY_ESC) {
                    sound_event = SE_KeyConfirm;
                    editMode = 0;
					buttons.action_down &= ~KEY_ESC;
                } else if (editorCodeValid) {
                    temp8u = processEditor(&edit, editorCode);
                    sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
				}
            }
            // Display
            if (!editMode) {
                if (wave_edit_mode == EDIT_LOW_AMP)
                    temp32u = DAC_GetSettingWaveMin();
                else
                    temp32u = DAC_GetSettingWaveMax();

                i32toa_align_right(temp32u, str, 10, 4, 3);
                LCD_InsertCharsXY(13, 2, &str[2], 5);
            } else {
                temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
                i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
				if (blink_flag)
					LCD_InsertCharsXY(14, 2, &str[5], 4);
				else 
					LCD_InsertCharsXY(14, 2, spaces, 4);
				
            }
            // Next item
            if (buttons.action_down & KEY_OUTPUT_CONST) {
                new_state = MENU_CONST_OUTPUT;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & (KEY_OK | KEY_ESC | KEY_OUTPUT_WAVE)) {
                new_state = MENU_WAVEFORM_OUTPUT;
                sound_event = SE_KeyConfirm;
            }
            break;
    }

    if (new_state != menu_state) {
        menu_state = new_state;
        first_visit = 1;
        editMode = 0;
    } else {
        first_visit = 0;
    }
}




static void drawSysMenuHeader(void) {
    LCD_Clear();
    LCD_PutStringXY(0,0,"Системные настройки:");
}

static void drawCalibrateDACMenuHeader(void) {
    LCD_Clear();
    LCD_PutStringXY(0,0,"Источник тока:");
}

static void drawCalibrateExtADCMenuHeader(void) {
    LCD_Clear();
    LCD_PutStringXY(0,0,"Амперметр:");
}


static void runCalibrationMode(void) {
    int32_t temp32;
    uint8_t temp8u;
    uint8_t new_state = menu_state;
    char str[10];
    encodeEditorKeys();

    switch (menu_state) {
        case SYS_CONTRAST:
            // Display and adjust contrast setting
            if (first_visit) {
                drawSysMenuHeader();
                LCD_PutStringXY(0,2,"Контраст (0-20):    ");
            }
            if (encoder_delta == 0) {
                if (buttons.action_down & KEY_NUM1) encoder_delta = -1;
                else if (buttons.action_down & KEY_NUM2) encoder_delta = 1;
            }

            if (encoder_delta) {
                temp32 = LCD_GetContrastSetting() + encoder_delta;
                temp8u = LCD_SetContrastSetting(temp32);
                sound_event = (temp8u == VALUE_IN_RANGE) ? SE_EncoderConfirm : SE_EncoderIllegal;
            }
            temp32 = LCD_GetContrastSetting();
            i32toa_align_right(temp32 , str, 10, 1, -1);
            LCD_InsertCharsXY(18, 2, &str[7], 2);
            // Next item
            if (buttons.action_down & KEY_OK) {
                new_state = SYS_BEEPER;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_BEEPER;
                sound_event = SE_KeyConfirm;
			}
            break;
        case SYS_BEEPER:
            // Display and adjust beeper setting
            if (first_visit) {
                drawSysMenuHeader();
                LCD_PutStringXY(0,2,"Звук: ");
            }
            if (encoder_delta == 0) {
                if (buttons.action_down & KEY_NUM1) encoder_delta = -1;
                else if (buttons.action_down & KEY_NUM2) encoder_delta = 1;
            }
            if (encoder_delta) {
                temp32 = Sound_GetEnabled() + encoder_delta;
                Sound_SetEnabled((temp32 <= 0) ? 0 : 1);
                sound_event = SE_KeyConfirm;
            }
            temp32 = Sound_GetEnabled();
            if (temp32 != 0)
                LCD_PutStringXY(15,2," вкл");
            else
                LCD_PutStringXY(15,2,"выкл");
            // Next item
            if (buttons.action_down & KEY_OK) {
                new_state = SYS_DAC_CALIBRATION_LOW;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_DAC_CALIBRATION_LOW;
                sound_event = SE_KeyConfirm;
			}
            break;
        case SYS_DAC_CALIBRATION_LOW:
            // Display and adjust DAC calibration point 1
            if (first_visit) {
                drawCalibrateDACMenuHeader();
                LCD_PutStringXY(0,1,"Калибровка 4мА");
                LCD_PutStringXY(0,2,"Измеренное значение:");
                LCD_PutStringXY(18,3,"мА");
                // Set current for first calibration point
                DAC_SetCalibrationPoint(1);
                startEditor(&edit, EDIT_NUM0, 3, 50000);
            }
            // User input
            if (editorCodeValid) {
                temp8u = processEditor(&edit, editorCode);
                sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
            }
            temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
            i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
            LCD_InsertCharsXY(11, 3, &str[2], 7);
            // Next item
            if (buttons.action_down & KEY_OK) {
                //temp32 = 4000;
                temp32 = getScaledEditValue(&edit);
                DAC_SaveCalibrationPoint(1, temp32);
                ADC_SaveLoopCurrentCalibrationPoint(1, temp32);
                new_state = SYS_DAC_CALIBRATION_HIGH;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_DAC_CALIBRATION_HIGH;
                sound_event = SE_KeyConfirm;
			}
            break;
        case SYS_DAC_CALIBRATION_HIGH:
            if (first_visit) {
                drawCalibrateDACMenuHeader();
                LCD_PutStringXY(0,1,"Калибровка 20мА");
                LCD_PutStringXY(0,2,"Измеренное значение:");
                LCD_PutStringXY(18,3,"мА");
                // Set current for second calibration point
                DAC_SetCalibrationPoint(2);
                startEditor(&edit, EDIT_NUM0, 3, 50000);
            }
            // User input
            if (editorCodeValid) {
                temp8u = processEditor(&edit, editorCode);
                sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
            }
            temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
            i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
            LCD_InsertCharsXY(11, 3, &str[2], 7);
            // Next item
            if (buttons.action_down & KEY_OK) {
                //temp32 = 20000;
                temp32 = getScaledEditValue(&edit);
                DAC_SaveCalibrationPoint(2, temp32);
                DAC_Calibrate();
                ADC_SaveLoopCurrentCalibrationPoint(2, temp32);
                ADC_LoopCurrentCalibrate();
                new_state = SYS_ADC_VOLTAGE_CALIBRATION_LOW;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_ADC_VOLTAGE_CALIBRATION_LOW;
                sound_event = SE_KeyConfirm;
			}
            break;
        case SYS_ADC_VOLTAGE_CALIBRATION_LOW:
            // Display and adjust V ADC calibration point 1
            if (first_visit) {
                drawCalibrateDACMenuHeader();
                LCD_PutStringXY(0,1,"Калибровка 4В");
                LCD_PutStringXY(0,2,"Измеренное значение:");
                LCD_PutStringXY(18,3,"В");
                // Set current for first calibration point
                DAC_UpdateOutput(4000);
                startEditor(&edit, EDIT_NUM0, 3, 50000);
            }
            // User input
            if (editorCodeValid) {
                temp8u = processEditor(&edit, editorCode);
                sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
            }
            temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
            i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
            LCD_InsertCharsXY(11, 3, &str[2], 7);
            // Next item
            if (buttons.action_down & KEY_OK) {
                // temp32 = 4000;
                temp32 = getScaledEditValue(&edit);
                ADC_SaveLoopVoltageCalibrationPoint(1, temp32);
                new_state = SYS_ADC_VOLTAGE_CALIBRATION_HIGH;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_ADC_VOLTAGE_CALIBRATION_HIGH;
                sound_event = SE_KeyConfirm;
			}
            break;
        case SYS_ADC_VOLTAGE_CALIBRATION_HIGH:
            // Display and adjust V ADC calibration point 2
            if (first_visit) {
                drawCalibrateDACMenuHeader();
                LCD_PutStringXY(0,1,"Калибровка 20В");
                LCD_PutStringXY(0,2,"Измеренное значение:");
                LCD_PutStringXY(18,3,"В");
                // Set current for first calibration point
                DAC_UpdateOutput(20000);
                startEditor(&edit, EDIT_NUM0, 3, 50000);
            }
            // User input
            if (editorCodeValid) {
                temp8u = processEditor(&edit, editorCode);
                sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
            }
            temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
            i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
            LCD_InsertCharsXY(11, 3, &str[2], 7);
            // Next item
            if (buttons.action_down & KEY_OK) {
                // temp32 = 20000;
                temp32 = getScaledEditValue(&edit);
                ADC_SaveLoopVoltageCalibrationPoint(2, temp32);
                ADC_LoopVoltageCalibrate();
                new_state = SYS_EXTADC_CALIBRATION_ZERO;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_EXTADC_CALIBRATION_ZERO;
                sound_event = SE_KeyConfirm;
			}
            break;
        case SYS_EXTADC_CALIBRATION_ZERO:
            // Display ampermeter zero calibration
            if (first_visit) {
                drawCalibrateExtADCMenuHeader();
                LCD_PutStringXY(0,1,"Калибровка 0мА");
            }
            // No user input
            // Next item
            if (buttons.action_down & KEY_OK) {
                // Save zero calibration point
                ExtADC_SaveCalibrationPoint(1, 0);
                new_state = SYS_EXTADC_CALIBRATION_LOW_RANGE;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_EXTADC_CALIBRATION_LOW_RANGE;
                sound_event = SE_KeyConfirm;
			}
            break;
        case SYS_EXTADC_CALIBRATION_LOW_RANGE:
            // Display and adjust ampermeter calibration point for low range
            if (first_visit) {
                drawCalibrateExtADCMenuHeader();
                LCD_PutStringXY(0,1,"Калибровка 40мА");
                LCD_PutStringXY(0,2,"Измеренное значение:");
                LCD_PutStringXY(18,3,"мА");
                // Set current for first calibration point
                DAC_UpdateOutput(20000);
                startEditor(&edit, EDIT_NUM0, 3, 1000000);
            }
            // User input
            if (editorCodeValid) {
                temp8u = processEditor(&edit, editorCode);
                sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
            }
            temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
            i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
            LCD_InsertCharsXY(11, 3, &str[2], 7);
            // Next item
            if (buttons.action_down & KEY_OK) {
                // temp32 = 20000;
                temp32 = getScaledEditValue(&edit);
                ExtADC_SaveCalibrationPoint(2, temp32);
                new_state = SYS_EXTADC_CALIBRATION_HIGH_RANGE;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_EXTADC_CALIBRATION_HIGH_RANGE;
                sound_event = SE_KeyConfirm;
			}
            break;
        case SYS_EXTADC_CALIBRATION_HIGH_RANGE:
            // Display and adjust ampermeter calibration point for low range
            if (first_visit) {
                drawCalibrateExtADCMenuHeader();
                LCD_PutStringXY(0,1,"Калибровка 400мА");
                LCD_PutStringXY(0,2,"Измеренное значение:");
                LCD_PutStringXY(18,3,"мА");
                // Set current for first calibration point
                DAC_UpdateOutput(20000);
                startEditor(&edit, EDIT_NUM0, 3, 1000000);
            }
            // User input
            if (editorCodeValid) {
                temp8u = processEditor(&edit, editorCode);
                sound_event = (temp8u == EDIT_OK) ? SE_KeyConfirm : SE_KeyReject;
            }
            temp8u = (edit.entered_digits > 0) ? edit.entered_digits : 1;
            i32toa_align_right(edit.value , str, 10, temp8u, edit.dot_position);
            LCD_InsertCharsXY(11, 3, &str[2], 7);
            // Next item
            if (buttons.action_down & KEY_OK) {
                // temp32 = 20000;
                temp32 = getScaledEditValue(&edit);
                ExtADC_SaveCalibrationPoint(3, temp32);
                ExtADC_Calibrate();
                new_state = SYS_DONE;
                sound_event = SE_KeyConfirm;
            } else if (buttons.action_down & KEY_ESC) {
				new_state = SYS_DONE;
                sound_event = SE_KeyConfirm;
			}
            break;
        default:
            if (first_visit) {
                LCD_Clear();
                LCD_PutStringXY(0,0,"Калибровка прибора");
                LCD_PutStringXY(0,1,"     завершена.");
                LCD_PutStringXY(0,3,"Перезапустите прибор");
				DAC_UpdateOutput(0);
            }
            break;
    }
    if (new_state != menu_state) {
        menu_state = new_state;
        first_visit = 1;
    } else {
        first_visit = 0;
    }
}



