
#include <stdint.h>


typedef struct {
    uint32_t max_value;
    uint32_t value;
    int8_t dot_position;
    uint8_t entered_digits;
    uint8_t fract_digits;
} edit_t;

enum EditorCodes {
    EDIT_NUM0,  EDIT_NUM1,  EDIT_NUM2,  EDIT_NUM3,
    EDIT_NUM4,  EDIT_NUM5,  EDIT_NUM6,  EDIT_NUM7,
    EDIT_NUM8,  EDIT_NUM9,  EDIT_DOT,   EDIT_BKSPACE
};

enum EditorResults {EDIT_OK, EDIT_AT_MAX};

void resetEditor(edit_t *edit, uint8_t numFractDigits);
void startEditor(edit_t *edit, uint8_t code, uint8_t numFractDigits, uint32_t maxValue);
uint8_t processEditor(edit_t *edit, uint8_t code);
uint32_t getScaledEditValue(edit_t *edit);
