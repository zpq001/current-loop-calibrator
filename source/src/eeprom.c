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
#include "eeprom_utils.h"


#define CRC_INITIAL_VALUE	0xFFFF
#define EEMEM_ADDRESS		0x08000000	// using information memory

settings_t settings;
system_settings_t system_settings;

static uint8_t eeprom_sector;
static uint32_t num_power_cycles;



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
	uint16_t i, j;
	uint8_t result = 0;
	uint16_t crc;
	uint32_t *ptr32;
	uint8_t *ptr8;
	uint32_t address;
	uint16_t size = sizeof(settings_t);
	size /= 4;	// using 32-bit word access
	
	num_power_cycles = 0;
	
	// Find more recent data
	for (j=0; j<4; j++) {

		if (j < 3) {
			address = EEMEM_ADDRESS + j*4;
		} else {
			if (num_power_cycles == 0) {
				// Not found correct data
				result = 0;
				break;
			}
			address = EEMEM_ADDRESS + eeprom_sector*4;
		}
		
		// Read data
		ptr32 = (uint32_t *)&settings;
		for (i=0; i<size; i++) {
			*ptr32++ = EEPROM_ReadWord(address, EEPROM_Info_Bank_Select);
			//address+=4;
			address += 0x10;	// using only one sector
		}
		
		// Check CRC
		crc = 0xFFFF;
		ptr8 = (uint8_t *)&settings;
		for (i=0; i<sizeof(settings); i++) {
			crc = crc16_update(crc, *ptr8++);
			if (ptr8 == (uint8_t *)&settings.crc)
				break;
		}
		
		if (settings.crc == crc) {
			if (settings.num_power_cycles > num_power_cycles) {
				num_power_cycles = settings.num_power_cycles;
				eeprom_sector = j;
			}
			result = 1;
		} else {
			result = 0;
		}
	}
	
	if (result != 0) {
		// Use next sector
		eeprom_sector = (eeprom_sector == 2) ? 0 : eeprom_sector + 1;
	} else {
		eeprom_sector = 0;
		num_power_cycles = 1;
	}
	
	// Erase next EEPROM sector to allow fast data saving
	EEPROM_ErasePageSector(EEMEM_ADDRESS, EEPROM_Info_Bank_Select, eeprom_sector);
	
	return result;
}


// Returns 1 is success
// 0 if CRC does not match
uint8_t EE_RestoreSystemSettings(uint8_t erase_current) {
	uint16_t i;
	uint8_t result = 0;
	uint16_t crc;
	uint32_t *ptr32;
	uint8_t *ptr8;
	uint32_t address;
	uint16_t size = sizeof(system_settings);
	size /= 4;	// using 32-bit word access
	
	// Read data
	address = EEMEM_ADDRESS + 0x0C;	// using sector D
	ptr32 = (uint32_t *)&system_settings;
	for (i=0; i<size; i++) {
		*ptr32++ = EEPROM_ReadWord(address, EEPROM_Info_Bank_Select);
		address += 0x10;	// using only one sector
	}
	
	// Check CRC
	crc = 0xFFFF;
	ptr8 = (uint8_t *)&system_settings;
	for (i=0; i<sizeof(system_settings); i++) {
		crc = crc16_update(crc, *ptr8++);
		if (ptr8 == (uint8_t *)&system_settings.crc)
			break;
	}
	
	if (system_settings.crc == crc) {
		result = 1;
	} else {
		result = 0;
	}

	if (erase_current) {
		// Erase EEPROM sector to allow fast data saving
		EEPROM_ErasePageSector(EEMEM_ADDRESS, EEPROM_Info_Bank_Select, 3);
	}
	return result;
}



void EE_SaveSettings(void) {
	uint16_t i;
	uint16_t crc;
	uint32_t *ptr32;
	uint8_t *ptr8;
	uint16_t size;
	uint32_t address;
	
	settings.num_power_cycles = num_power_cycles + 1;
	size = sizeof(settings);
	
	// Get CRC
	crc = 0xFFFF;
	ptr8 = (uint8_t *)&settings;
	for (i=0; i<size; i++) {
		crc = crc16_update(crc, *ptr8++);
		if (ptr8 == (uint8_t *)&settings.crc)
			break;
	}
	settings.crc = crc;
	
	// Erase page
	//EEPROM_ErasePage(EEMEM_ADDRESS, EEPROM_Info_Bank_Select);
	// One sector is 1Kx8
	//EEPROM_ErasePageSector(EEMEM_ADDRESS, EEPROM_Info_Bank_Select, 0);	
	
	ptr32 = (uint32_t *)&settings;
	address = EEMEM_ADDRESS + eeprom_sector*4;
	for (i=0; i<size/4; i++) {
		EEPROM_ProgramWord(address, EEPROM_Info_Bank_Select, *ptr32++);
		//address += 4;
		address += 0x10;	// using only sector A
	}
}


void EE_SaveSystemSettings(void) {
	uint16_t i;
	uint16_t crc;
	uint32_t *ptr32;
	uint8_t *ptr8;
	uint16_t size;
	uint32_t address;
	
	size = sizeof(system_settings);
	
	// Get CRC
	crc = 0xFFFF;
	ptr8 = (uint8_t *)&system_settings;
	for (i=0; i<size; i++) {
		crc = crc16_update(crc, *ptr8++);
		if (ptr8 == (uint8_t *)&system_settings.crc)
			break;
	}
	system_settings.crc = crc;
		
	
	ptr32 = (uint32_t *)&system_settings;
	address = EEMEM_ADDRESS + 0x0C;
	for (i=0; i<size/4; i++) {
		EEPROM_ProgramWord(address, EEPROM_Info_Bank_Select, *ptr32++);
		address += 0x10;	// using only one sector
	}
}






