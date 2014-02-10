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
#include "WAIT.h"

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

void* DB_GetVar(uint8_t varID) {
	return database[varID].var_ptr;
}

uint8_t DB_GetVar_u8(uint8_t varID) {
	return *(uint8_t*) DB_GetVar(varID);
}

// function: all to eeprom
