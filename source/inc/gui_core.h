
#include <stdint.h>

//--------------------------------------------//
// Typedefs

typedef void (*FuncPtr)(void);

typedef struct {
	uint8_t		ItemId;				//
	uint32_t	JumpCondition;      //
	uint8_t		NextItemId;			//
	uint8_t		ConditionCheckType;
} MenuJumpRecord_t;


typedef struct {
	uint8_t				ItemId;				//
	FuncPtr				SelectFunction;     //
	FuncPtr				RunFunction;		//
	FuncPtr				LeaveFunction;      //
} MenuFunctionRecord_t;

enum ItemFunctionTypes { SELECT_FUNCTION, RUN_FUNCTION, LEAVE_FUNCTION };

#define JCONDITIONS_MASK    0x0F
#define JKEYACTIONS_MASK    0xF0

enum JumpContitionCheckTypes {
    JUMP_IF_EXACT = 0x00,
    JUMP_IF_ANY,
    JUMP_ALWAYS
};

enum JumpKeyActions {
    KEY_ACT_DOWN = 0x10,
    KEY_ACT_UP_SHORT = 0x20,
    KEY_ACT_HOLD = 0x30
};

typedef struct {
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


uint8_t getNextMenuItem(uint8_t selectedItemId);
const MenuFunctionRecord_t* getMenuFunctionRecord(uint8_t menuItemId);
void processItemFunction(const MenuFunctionRecord_t* funcRecord, uint8_t funcType);

void resetEditor(edit_t *edit, uint8_t numFractDigits);
void startEditor(edit_t *edit, uint8_t code, uint8_t numFractDigits);
void processEditor(edit_t *edit, uint8_t code);
uint32_t getScaledEditValue(edit_t *edit, uint8_t scale);
