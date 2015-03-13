
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

enum JumpContitionCheckTypes {
    JUMP_IF_EXACT,
    JUMP_IF_ANY,
    JUMP_ALWAYS
};

uint8_t getNextMenuItem(uint8_t selectedItemId, uint32_t jmpCond);
const MenuFunctionRecord_t* getMenuFunctionRecord(uint8_t menuItemId);
void processItemFunction(const MenuFunctionRecord_t* funcRecord, uint8_t funcType);
