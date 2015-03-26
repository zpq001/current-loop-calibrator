
#include <string.h>
#include "gui_core.h"


void resetEditor(edit_t *edit, uint8_t numFractDigits) {
    edit->value = 0;
    edit->dot_position = -1;
    edit->entered_digits = 0;
    edit->fract_digits = numFractDigits;
}


void startEditor(edit_t *edit, uint8_t code, uint8_t numFractDigits, uint32_t maxValue) {
    resetEditor(edit, numFractDigits);
    edit->max_value = maxValue;
    processEditor(edit, code);
}


uint8_t processEditor(edit_t *edit, uint8_t code) {
    uint32_t temp32u;
    int8_t temp8;
    uint8_t result = EDIT_OK;
    if (code <= EDIT_NUM9) {
        if ((code != EDIT_NUM0) || (edit->value != 0) || (edit->dot_position >= 0)) {
            if (edit->dot_position < edit->fract_digits) {
                temp32u = edit->value;
                temp8 = edit->dot_position;
                edit->value *= 10;
                edit->value += code;
                edit->entered_digits++;
                if(edit->dot_position >= 0)
                    edit->dot_position++;
                }
                // Check maximum
                if (getScaledEditValue(edit) > edit->max_value) {
                    // Revert
                    edit->value = temp32u;
                    edit->dot_position = temp8;
                    edit->entered_digits--;
                    result = EDIT_AT_MAX;
                }
        }
    } else if (code == EDIT_DOT) {
        if (edit->dot_position < 0) {
            edit->dot_position = 0;
            if (edit->value == 0) {
                edit->entered_digits++;
            }
        }
    } else if (code == EDIT_BKSPACE) {
        if (edit->dot_position == 0) {
            edit->dot_position--;
            if (edit->value == 0)
                edit->entered_digits = 0;
        } else {
            edit->value /= 10;
            if (edit->entered_digits) {
                edit->entered_digits--;
                if (edit->dot_position > 0)
                    edit->dot_position--;
            }
        }
    }
    return result;
}

uint32_t getScaledEditValue(edit_t *edit) {
    uint32_t temp32u;
    uint8_t scale = edit->fract_digits;
    temp32u = edit->value;
    if (edit->dot_position > 0)
        scale -= edit->dot_position;

    while(scale > 0) {
        temp32u *= 10;
        scale--;
    }

    return temp32u;
}





