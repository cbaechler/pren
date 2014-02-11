/**
 * \file
 * \brief Database Module.
 * \author Christoph Bächler
 * \date 10.02.2014
 *
 * This file contains function for database access. The database 
 * structure allows to save addresses (pointers) inside its 
 * array. This allows to read and store data to the eeprom and 
 * makes it easy to change contents of the vars through the 
 * binary protocol.  
 */

#include "PE_Types.h"
#include "Database.h"
#include "NVM.h"

DB_Var database[DB_NOF_VARS];

void DB_Init(void) {
	uint8_t i;
	for(i=0; i<DB_NOF_VARS; i++) {		// set defaults for each varaible in database
		database[i].var_ptr = NULL;		// empty pointer
		database[i].ee = FALSE;			// do not store to eeprom
	}
}

void DB_RegisterVar(uint8_t varID, void* adr, DB_DataType type, bool eeprom) {
	EnterCritical();
	database[varID].var_ptr = adr;
	database[varID].type = type;
	database[varID].ee = eeprom;
	ExitCritical();
}

DB_DataType DB_GetType(uint8_t varID) {
	return database[varID].type;
}

uint8_t DB_GetTypeSize(DB_DataType type) {
	switch(type) {
		case U8: 	return sizeof(uint8_t);
		case U16:	return sizeof(uint16_t);
	}
	
	return 0;
}

void* DB_GetVar(uint8_t varID) {
	return database[varID].var_ptr;
}

uint8_t DB_GetVar_u8(uint8_t varID) {
	return *(uint8_t*) DB_GetVar(varID);
}

void DB_LoadEEPROM(void) {
	uint8_t i, j;
	uint32_t nvm_pos = (uint32_t) DB_NVM_BASE_ADDR;
	for(i=0; i<DB_NOF_VARS; i++) {
		if(database[i].ee) {
			for(j=0; j<DB_GetTypeSize(database[i].type); j++) {
				NVM_GetByteFlash((NVM_TAddress)(nvm_pos+j), (uint8_t*) (database[i].var_ptr+j));
			}

			nvm_pos += DB_GetTypeSize(database[i].type);

			/* == first version of the code:
			switch(database[i].type) {
				case U8: {
					NVM_GetByteFlash((NVM_TAddress)nvm_pos, (uint8_t*) database[i].var_ptr);
					break;
				}
				case U16: {
					NVM_GetWordFlash((NVM_TAddress)nvm_pos, (uint16_t*) database[i].var_ptr);				
					break;
				}
			}

			nvm_pos += DB_GetTypeSize(database[i].type);*/
		}
	}
}

void DB_SaveEEPROM(void) {
	uint8_t i, j;
	uint32_t nvm_pos = (uint32_t) DB_NVM_BASE_ADDR;
	for(i=0; i<DB_NOF_VARS; i++) {
		if(database[i].ee) {
			for(j=0; j<DB_GetTypeSize(database[i].type); j++) {
				NVM_SetByteFlash((NVM_TAddress)(nvm_pos+j), *(uint8_t*) (database[i].var_ptr+j));
			}

			nvm_pos += DB_GetTypeSize(database[i].type);
			
			/* == first version of the code:
			switch(database[i].type) {
				case U8: {
					NVM_SetByteFlash((NVM_TAddress)nvm_pos, *(uint8_t*) database[i].var_ptr);
				}
				case U16: {
					NVM_SetWordFlash((NVM_TAddress)nvm_pos, *(uint16_t*) database[i].var_ptr);
					break;
				}
			}
			
			nvm_pos += DB_GetTypeSize(database[i].type);*/
		}
	}
}

// function: all to eeprom
