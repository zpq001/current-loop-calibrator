
#include <string.h>
#include "gui_core.h"

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
uint8_t getNextMenuItem(uint8_t selectedItemId, uint32_t jmpCond) {
	uint8_t nextItemId = selectedItemId;
    uint8_t jumpConditionMatch = 0;
	uint8_t i = 0;
    while ((menuJumpSet[i].ItemId) && (!jumpConditionMatch)) {
        if (menuJumpSet[i].ItemId == selectedItemId) {
            switch (menuJumpSet[i].ConditionCheckType) {
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

