/*******************************************************************
	Module eeprom.c
	



Make sure EEPROM functions from SPL are located in RAM.
Specify that in dialog window for file MDR32F9Qx_eeprom.c
Also make sure Use Memory Layout from Target Dialog in the dialog Project - Options for Target - Linker is enabled
http://www.keil.com/support/man/docs/gsac/gsac_specificmemoryareas.htm
Set correct FLASH_PROG_FREQ_MHZ in config to ensure proper SPL operation

********************************************************************/

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_eeprom.h"
#include "eeprom.h"


#define CRC_INITIAL_VALUE	0xFFFF
#define EEMEM_ADDRESS		0x08000000	// using information memory

settings_t settings;




static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
    int i;
    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }
    return crc;
}


// Returns 1 is success
// 0 if CRC does not match
uint8_t EE_RestoreSettings(void) {
	uint16_t i;
	uint16_t crc;
	uint16_t size = sizeof(settings_t);
	uint32_t *ptr32 = (uint32_t *)&settings;
	uint8_t *ptr8;
	uint32_t address = EEMEM_ADDRESS;
	//uint32_t address = 0x080000C0;
	size /= 4;	// using 32-bit word access
	
	for (i=0; i<size; i++) {
		*ptr32++ = EEPROM_ReadWord(address, EEPROM_Info_Bank_Select);
		//*ptr32++ = EEPROM_ReadWord(address, EEPROM_Main_Bank_Select);
		address+=4;
	}
	
	// Check CRC
	crc = 0xFFFF;
	ptr8 = (uint8_t *)&settings;
	for (i=0; i<sizeof(settings); i++) {
		crc = crc16_update(crc, *ptr8++);
		if (ptr8 == (uint8_t *)&settings.crc)
			break;
	}
	
	return (settings.crc == crc);
}



void EE_SaveSettings(void) {
	uint16_t i;
	uint16_t crc;
	uint32_t *ptr32 = (uint32_t *)&settings;
	uint8_t *ptr8;
	uint16_t size;
	uint32_t address;
	
	// Get CRC
	crc = 0xFFFF;
	ptr8 = (uint8_t *)&settings;
	for (i=0; i<sizeof(settings); i++) {
		crc = crc16_update(crc, *ptr8++);
		if (ptr8 == (uint8_t *)&settings.crc)
			break;
	}
	settings.crc = crc;
	
	// Erase page
	EEPROM_ErasePage(EEMEM_ADDRESS, EEPROM_Info_Bank_Select);
	
	size = sizeof(settings);
	ptr32 = (uint32_t *)&settings;
	address = EEMEM_ADDRESS;
	size /= 4;	// using 32-bit word access
	for (i=0; i<size; i++) {
		EEPROM_ProgramWord(address, EEPROM_Info_Bank_Select, *ptr32++);
		address += 4;
	}
}







