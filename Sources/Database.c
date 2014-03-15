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
#include "Application.h"
#include "Database.h"
#include "Motors.h"
#include "BlockStack.h"
#include "Serial.h"
#include "NVM.h"

DB_Var database[DB_NOF_VARS];

void DB_Init(void) {
	uint8_t i;
	for(i=0; i<DB_NOF_VARS; i++) {		// set defaults for each varaible in database
		database[i].var_ptr = NULL;		// empty pointer
		database[i].nvm = FALSE;			// do not store to eeprom
	}
	
	// Register Global Variables (all nvm variables must be registered here)
	DB_RegisterVar(DB_DBGBUFFER, &debugBuffer, T_DBGBUFFER, TRUE);
	DB_RegisterVar(DB_MOT_ROTARY, &(rotary.p), MOT, TRUE);
	DB_RegisterVar(DB_MOT_KNEE, &(knee.p), MOT, TRUE);
	DB_RegisterVar(DB_MOT_LIFT, &(lift.p), MOT, TRUE);
	DB_RegisterVar(DB_BLOCK_ZBLOCKHEIGHT, &(zBlockHeight), U16, TRUE);
	DB_RegisterVar(DB_BLOCK_ZTARGETSURFACE, &(zTargetSurface), U16, TRUE);
	DB_RegisterVar(DB_BLOCK_ZGROUNDSURFACE, &(zGroundSurface), U16, TRUE);
		
	// Load values of registered globals from NVM
	DB_LoadNVM();
}

void DB_RegisterVar(uint8_t varID, void* adr, DB_DataType type, bool eeprom) {
	EnterCritical();
	database[varID].var_ptr = adr;
	database[varID].type = type;
	database[varID].nvm = eeprom;
	ExitCritical();
}

DB_DataType DB_GetType(uint8_t varID) {
	return database[varID].type;
}

uint8_t DB_GetTypeSize(DB_DataType type) {
	switch(type) {
		case U8: 	return sizeof(uint8_t);
		case U16:	return sizeof(uint16_t);
		case MOT:	return sizeof(MOT_PubData);
		case T_DBGBUFFER: return SER_DEBUGBUFFER_LENGTH;
	}
	
	return 0;
}

void* DB_GetVar(uint8_t varID) {
	return database[varID].var_ptr;
}

uint8_t DB_GetVar_u8(uint8_t varID) {
	return *(uint8_t*) DB_GetVar(varID);
}

void DB_LoadNVM(void) {
	uint8_t i, j;
	uint32_t nvm_pos = (uint32_t) DB_NVM_BASE_ADDR;
	for(i=0; i<DB_NOF_VARS; i++) {
		if(database[i].nvm) {
			for(j=0; j<DB_GetTypeSize(database[i].type); j++) {
				NVM_GetByteFlash((NVM_TAddress)(nvm_pos+j), (uint8_t*) (database[i].var_ptr+j));
			}

			nvm_pos += DB_GetTypeSize(database[i].type);
		}
	}
}

void DB_SaveNVM(void) {
	uint8_t i, j;
	uint32_t nvm_pos = (uint32_t) DB_NVM_BASE_ADDR;
	for(i=0; i<DB_NOF_VARS; i++) {
		if(database[i].nvm) {
			for(j=0; j<DB_GetTypeSize(database[i].type); j++) {
				NVM_SetByteFlash((NVM_TAddress)(nvm_pos+j), *(uint8_t*) (database[i].var_ptr+j));
			}

			nvm_pos += DB_GetTypeSize(database[i].type);
		}
	}
}
