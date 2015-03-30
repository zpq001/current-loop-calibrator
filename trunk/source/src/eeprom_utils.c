/****************************************************************//*
	@brief Module EEPROM utils
	
	Extends basic functionality provided by SPL
    
    
********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_eeprom.h"

#define EEPROM_REG_ACCESS_KEY           ((uint32_t)0x8AAA5551)
#define IS_TWO_BYTE_ALLIGNED(ADDR)      ((ADDR & 1) == 0)
#define IS_FOUR_BYTE_ALLIGNED(ADDR)     ((ADDR & 3) == 0)

#define DELAY_LOOP_CYCLES               (8UL)
#define GET_US_LOOPS(N)                 ((uint32_t)((float)(N) * FLASH_PROG_FREQ_MHZ / DELAY_LOOP_CYCLES))


__RAMFUNC static void ProgramDelay(uint32_t Loops) __attribute__((section("EXECUTABLE_MEMORY_SECTION")));

/**
  * @brief  Program delay.
  * @param  Loops: Number of the loops.
  * @retval None.
  */
__RAMFUNC static void ProgramDelay(uint32_t Loops)
{
  volatile uint32_t i = Loops;
  for (; i > 0; i--)
  {
  }
}



__RAMFUNC void EEPROM_ErasePageSector(uint32_t Address, uint32_t BankSelector, uint8_t sector)
{
  uint32_t Command;
  uint32_t Offset;

  //assert_param(IS_EEPROM_BANK_SELECTOR(BankSelector));

  MDR_EEPROM->KEY = EEPROM_REG_ACCESS_KEY;
  Command = (MDR_EEPROM->CMD & EEPROM_CMD_DELAY_Msk) | EEPROM_CMD_CON;
  Command |= (BankSelector == EEPROM_Info_Bank_Select) ? EEPROM_CMD_IFREN : 0;
  MDR_EEPROM->CMD = Command;

  Offset = sector*4;
  //for (Offset = 0; Offset < (4 << 2); Offset += 4)
  //{
    MDR_EEPROM->ADR = Address + Offset;             /* Page Address */
    MDR_EEPROM->DI = 0;
    Command |= EEPROM_CMD_XE | EEPROM_CMD_ERASE;
    MDR_EEPROM->CMD = Command;
    ProgramDelay(GET_US_LOOPS(5));              /* Wait for 5 us */
    Command |= EEPROM_CMD_NVSTR;
    MDR_EEPROM->CMD = Command;
    ProgramDelay(GET_US_LOOPS(40000));          /* Wait for 40 ms */
    Command &= ~EEPROM_CMD_ERASE;
    MDR_EEPROM->CMD = Command;
    ProgramDelay(GET_US_LOOPS(5));              /* Wait for 5 us */
    Command &= ~(EEPROM_CMD_XE | EEPROM_CMD_NVSTR);
    MDR_EEPROM->CMD = Command;
    ProgramDelay(GET_US_LOOPS(1));              /* Wait for 1 us */
  //}
  Command &= EEPROM_CMD_DELAY_Msk;
  MDR_EEPROM->CMD = Command;
  MDR_EEPROM->KEY = 0;
}



