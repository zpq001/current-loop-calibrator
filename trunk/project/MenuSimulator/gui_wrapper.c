
#include <string.h>
#include <stdio.h>
#include "buttons.h"
#include "gui_wrapper.h"
#include "gui_top.h"
#include "adc.h"
#include "external_adc.h"
#include "dac.h"
#include "power_monitor.h"
#include "sound.h"
#include "lcd_contrast.h"
#include "eeprom.h"

buttons_t buttons;
int16_t encoder_delta;
uint8_t contrastSetting;	// 0 to 20
uint8_t sound_enabled;
uint8_t device_mode;
system_settings_t system_settings;

struct {
    char *strings[4];    // + \0
    char str_data[4][21];
    uint8_t cursor_x;
    uint8_t cursor_y;
} lcd;


// Callback functions
cbLcdUpdatePtr updateLcdCallback;

static struct {
    uint32_t setting[DAC_PROFILE_COUNT];	// [uA]
    uint32_t dac_code;
    uint8_t profile;
    uint8_t mode;
    uint8_t waveform;
    uint8_t oe;
    uint8_t cycles_done;
    uint8_t regenerate_waveform;
    uint32_t period;		// [ms]
    uint32_t wave_min;		// [uA]
    uint32_t wave_max;		// [uA]
    uint32_t total_cycles;
    uint32_t current_cycle;
} dac_state;

//-----------------------------------//
// Callbacks top->GUI
void registerLcdUpdateCallback(cbLcdUpdatePtr fptr)
{
    updateLcdCallback = fptr;
}


void guiInitialize(void)
{
    uint8_t i;
    // Default state after power-on
    for (i=0; i<DAC_PROFILE_COUNT; i++)
        dac_state.setting[i] = 4000;
    dac_state.profile = 1;
    dac_state.mode = DAC_MODE_CONST;
    dac_state.waveform = WAVE_MEANDR;
    dac_state.period = 1500;
    dac_state.wave_min = 4000;
    dac_state.wave_max = 20000;
    dac_state.total_cycles = 95684;
    dac_state.current_cycle = 87521;

    device_mode = MODE_NORMAL;
    //device_mode = MODE_CALIBRATION;

    system_settings.output_mode = OUTPUT_UPDATE_NORMAL;

    contrastSetting = 10;
    sound_enabled = 1;


    for (i=0; i<4; i++)
    {
        lcd.strings[i] = lcd.str_data[i];
    }
    LCD_Clear();
    GUI_Init();
}

void guiButtonEvent(void)
{
    encoder_delta = 0;
    GUI_Process();

}

void guiUpdate(void)
{
    encoder_delta = 0;
    GUI_Process();
}


//-----------------------------------//
// Taps

void LCD_Clear(void) {
    uint8_t i;
    for (i=0; i<4; i++)
        snprintf(lcd.strings[i], 21, "                    ");
    lcd.cursor_x = 0;
    lcd.cursor_y = 0;
    updateLcdCallback((char**)lcd.strings);
}

void LCD_SetCursorPosition(uint8_t x, uint8_t y) {
    lcd.cursor_x = x;
    lcd.cursor_y = y;
}


void LCD_PutString(const char *data) {
    uint16_t len = strlen(data);
    char *tmp = lcd.strings[lcd.cursor_y];
    memcpy(&tmp[lcd.cursor_x], data, len);
    updateLcdCallback((char**)lcd.strings);
}

void LCD_InsertChars(const char *data, uint8_t count) {
    memcpy(&lcd.strings[lcd.cursor_y][lcd.cursor_x], data, count);
    updateLcdCallback((char**)lcd.strings);
}

void LCD_PutStringXY(uint8_t x, uint8_t y, const char *data) {
    LCD_SetCursorPosition(x, y);
    LCD_PutString(data);
    updateLcdCallback((char**)lcd.strings);
}

void LCD_InsertCharsXY(uint8_t x, uint8_t y, const char *data, uint8_t count) {
    LCD_SetCursorPosition(x, y);
    LCD_InsertChars(data, count);
    updateLcdCallback((char**)lcd.strings);
}



uint8_t ADC_GetLoopStatus(void) {
    return LOOP_OK;
}

uint32_t ADC_GetLoopCurrent(void) {
    return 21749;
}


void ADC_SaveLoopCurrentCalibrationPoint(uint8_t pointNum, uint32_t measuredValue) {
}

void ADC_LoopCurrentCalibrate(void) {
}

uint32_t ADC_GetLoopVoltage(void) {
    return 18351;
}

void ADC_SaveLoopVoltageCalibrationPoint(uint8_t pointNum, uint32_t measuredValue) {
}

void ADC_LoopVoltageCalibrate(void) {
}


int32_t ExtADC_GetCurrent(void) {
    //return 0;
    //return -415000;
    //return -380491;     // display as 380
    return  -35751;     // display as -35.8
}

uint8_t ExtADC_GetRange(void) {
    return EXTADC_LOW_RANGE;
    //return EXTADC_HIGH_RANGE;
    //return EXTADC_HIGH_OVERLOAD;
}

void ExtADC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue) {
}

void ExtADC_Calibrate(void) {

}




void DAC_RestoreSettings(void) {
}

void DAC_SaveSettings(void) {
}


// Enables or disables output in both constant source and
// waveform modes
void DAC_SetOutputState(uint8_t isEnabled) {
    if (isEnabled != dac_state.oe) {
        dac_state.oe = isEnabled;
    }
}

uint8_t DAC_SetSettingConst(int32_t value) {
    uint8_t result = verify_uint32(&value, DAC_MIN_SETTING, DAC_MAX_SETTING);
    dac_state.setting[dac_state.profile-1] = value;
    if (dac_state.mode == DAC_MODE_CONST) {
        DAC_UpdateOutput(dac_state.setting[dac_state.profile-1]);
    }
    return result;
}

void DAC_SetCalibrationPoint(uint8_t pointNumber) {
}

uint8_t DAC_SetProfile(uint32_t value) {
    uint8_t result = verify_uint32(&value, 1, DAC_PROFILE_COUNT);
    dac_state.profile = value;
    return result;
}

uint8_t DAC_SetSettingWaveMax(int32_t value) {
    uint8_t result = verify_uint32(&value, DAC_MIN_SETTING, DAC_MAX_SETTING);
    dac_state.wave_max = value;
    return result;
}

uint8_t DAC_SetSettingWaveMin(int32_t value) {
    uint8_t result = verify_uint32(&value, DAC_MIN_SETTING, DAC_MAX_SETTING);
    dac_state.wave_min = value;
    return result;
}

void DAC_SetWaveform(uint8_t newWaveForm) {
    dac_state.waveform = newWaveForm;
}

uint8_t DAC_SetPeriod(int32_t value) {
    uint8_t result = verify_uint32(&value, DAC_PERIOD_MIN, DAC_PERIOD_MAX);
    dac_state.period = value;
    return result;
}

void DAC_SetMode(uint8_t new_mode) {
    if (new_mode != dac_state.mode) {
        if (new_mode == DAC_MODE_WAVEFORM) {
            dac_state.mode = DAC_MODE_WAVEFORM;

        } else {
            dac_state.mode = DAC_MODE_CONST;
        }
    }
}

uint8_t DAC_SetTotalCycles(uint32_t value) {
    uint8_t result = verify_uint32(&value, DAC_CYCLES_MIN, DAC_CYCLES_MAX);
    dac_state.total_cycles = (value == 0) ? 1 : value;
    dac_state.current_cycle = 1;
    return result;
}

void DAC_RestartCycles(void) {
    dac_state.current_cycle = 1;
}





uint8_t DAC_GetOutputState(void) {
    return dac_state.oe;
}

uint32_t DAC_GetSettingConst(void) {
    return dac_state.setting[dac_state.profile-1];
}

uint8_t DAC_GetActiveProfile(void) {
    return dac_state.profile;
}

uint32_t DAC_GetSettingWaveMax(void) {
    return dac_state.wave_max;
}

uint32_t DAC_GetSettingWaveMin(void) {
    return dac_state.wave_min;
}

uint8_t DAC_GetWaveform(void) {
    return dac_state.waveform;
}

uint32_t DAC_GetPeriod(void) {
    return dac_state.period;
}

uint8_t DAC_GetMode(void) {
    return dac_state.mode;
}

uint32_t DAC_GetTotalCycles(void) {
    return dac_state.total_cycles;
}

uint32_t DAC_GetCurrentCycle(void) {
    return dac_state.current_cycle;
}

uint32_t DAC_GetCalibrationPoint(uint8_t pointNumber) {
    uint32_t temp32u;
    if (pointNumber == 1)
        temp32u = 4000;
    else
        temp32u = 20000;
    return temp32u;
}

// Sets DAC output to specified value [uA]
void DAC_UpdateOutput(uint32_t value) {
}


void DAC_SaveCalibrationPoint(uint8_t pointNum, uint32_t measuredValue) {
}


void DAC_Calibrate(void) {
}


uint8_t LCD_SetContrastSetting(int32_t value) {
    uint8_t result = verify_int32(&value, LCD_CONTRAST_MIN, LCD_CONTRAST_MAX);
    contrastSetting = value;
    return result;
}

uint8_t LCD_GetContrastSetting(void) {
    return contrastSetting;
}




void Sound_SetEnabled(uint8_t state) {
    sound_enabled = state;
}

uint8_t Sound_GetEnabled(void) {
    return sound_enabled;
}



void Sound_Event(uint8_t event) {
    if (sound_enabled) {
        switch (event) {
            case SE_Start:

                break;
            case SE_KeyConfirm:

                break;
            case SE_KeyReject:

                break;
            case SE_EncoderConfirm:

                break;
            case SE_EncoderIllegal:

                break;
            case SE_SettingConfirm:

                break;
            case SE_SettingIllegal:

                break;
            case SE_CyclesDone:

                break;
        }
    }
}
