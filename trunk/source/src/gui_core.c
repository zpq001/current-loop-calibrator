
#include <string.h>
#include "gui_core.h"
#include "buttons.h"

extern const MenuJumpRecord_t menuJumpSet[];
extern const MenuFunctionRecord_t menuFunctionSet[];

//-----------------------------------------------------------------//
//	Returns ID of next menu item. 
// If current item ID and jump condition match the
// records in the "menuJumpSet" table new ID is returned.
// If there is no record match, selectedItemId is returned.
//	Arguments:
//		selectedItemId	- ID of currently active item
//		jmpCond			- all information for jumps between states (buttons, timer flags, etc.)
//	Output:
//		NextItemID		- ID of item to jump to.
//-----------------------------------------------------------------//
uint8_t getNextMenuItem(uint8_t selectedItemId) {
	uint8_t nextItemId = selectedItemId;
    uint8_t jumpConditionMatch = 0;
	uint8_t i = 0;
    uint32_t jmpCond;
    volatile uint8_t keyAction;
    while ((menuJumpSet[i].ItemId) && (!jumpConditionMatch)) {
        if (menuJumpSet[i].ItemId == selectedItemId) {
            keyAction = menuJumpSet[i].ConditionCheckType & JKEYACTIONS_MASK;
            switch (keyAction) {
                case KEY_ACT_DOWN:      jmpCond = buttons.action_down;      break;
                case KEY_ACT_UP_SHORT:  jmpCond = buttons.action_up_short;  break;
                case KEY_ACT_HOLD:      jmpCond = buttons.action_hold;      break;
                default: jmpCond = 0;
            }
            switch (menuJumpSet[i].ConditionCheckType & JCONDITIONS_MASK) {
                case JUMP_IF_EXACT:
                    jumpConditionMatch = (menuJumpSet[i].JumpCondition == jmpCond) ? 1 : 0;
                    break;
                case JUMP_IF_ANY:
                    jumpConditionMatch = (menuJumpSet[i].JumpCondition & jmpCond) ? 1 : 0;
                    break;
                default:
                    jumpConditionMatch = 1;
            }
        }
        if (!jumpConditionMatch) i++;
    }
    if (jumpConditionMatch)
        nextItemId = menuJumpSet[i].NextItemId;
    
	return nextItemId;
}


//-----------------------------------------------------------------//
//	Finds function pointers record for menuItemID
//	If menuItemID is not found, 0 is returned.
//	Arguments:
//		menuItemID - ID of an item
//	Output:
//-----------------------------------------------------------------//
const MenuFunctionRecord_t* getMenuFunctionRecord(uint8_t menuItemId) {
    uint8_t i = 0;
    const MenuFunctionRecord_t *menuFunctionRecord = 0;
    while (menuFunctionSet[i].ItemId) {
        if (menuFunctionSet[i].ItemId == menuItemId) {
            menuFunctionRecord = &menuFunctionSet[i];
            break;
        }
        i++;
    }
    return menuFunctionRecord;
}


//-----------------------------------------------------------------//
//	Runs function at specified address
//	Arguments:
//		funcAddr - address of the function to run.
//-----------------------------------------------------------------//
void processItemFunction(const MenuFunctionRecord_t* funcRecord, uint8_t funcType) {
	if (funcRecord) {
        switch(funcType) {
            case SELECT_FUNCTION:
                if (funcRecord->SelectFunction) 
                    funcRecord->SelectFunction();
                break;
            case RUN_FUNCTION:
                if (funcRecord->RunFunction) 
                    funcRecord->RunFunction();
                break;
            case LEAVE_FUNCTION:
                if (funcRecord->LeaveFunction) 
                    funcRecord->LeaveFunction();
                break;
        }
    }
}


void resetEditor(edit_t *edit, uint8_t numFractDigits) {
    edit->value = 0;
    edit->dot_position = -1;
    edit->entered_digits = 0;
    edit->fract_digits = numFractDigits;
}

void startEditor(edit_t *edit, uint8_t code, uint8_t numFractDigits) {
    resetEditor(edit, numFractDigits);
    processEditor(edit, code);
}

void processEditor(edit_t *edit, uint8_t code) {
    if (code <= EDIT_NUM9) {
        if ((code != EDIT_NUM0) || (edit->value != 0) || (edit->dot_position >= 0)) {
            if (edit->dot_position < edit->fract_digits) {
            edit->value *= 10;
            edit->value += code;
            edit->entered_digits++;
            if(edit->dot_position >= 0)
                edit->dot_position++;
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
}

uint32_t getScaledEditValue(edit_t *edit, uint8_t scale) {
    uint32_t temp32u;

    temp32u = edit->value;
    if (edit->dot_position > 0)
        scale -= edit->dot_position;

    while(scale > 0) {
        temp32u *= 10;
        scale--;
    }

    return temp32u;
}





