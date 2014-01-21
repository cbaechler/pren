/**
 * \file
 * \brief Block Store module implementation.
 * \author Christoph Bächler
 *
 * BlockStorage is the database to store block positions. 
 * It is implemented as a simple stack containing 
 * robot arm positions for every block. The data 
 * structure allows a maximum of 10 blocks to be stored.
 */

#include "PE_Types.h"
#include "Cpu.h"
#include "BlockStack.h"

static BLOCK_Object block_storage[BLOCK_STACK_MAX_SIZE];
static uint8_t block_index;

void BLOCK_Init(void) {
	BLOCK_Clear();
}

bool BLOCK_IsEmtpy(void) {
	return block_index < 0;
}

bool BLOCK_IsFull(void) {
	return block_index > BLOCK_STACK_MAX_SIZE-1;
}

void BLOCK_Push(BLOCK_Object obj) {
	if(!BLOCK_IsFull()) {
		EnterCritical();
		block_storage[++block_index] = obj;
		ExitCritical();
	}	
}

BLOCK_Object BLOCK_Pop(void) {
	BLOCK_Object obj;
	if(!BLOCK_IsEmtpy()) {
		EnterCritical();
		obj = BLOCK_GetSingle(block_index--);
		ExitCritical();
	}
	return obj;
}

BLOCK_Object BLOCK_GetSingle(uint8_t index) {
	return block_storage[index];
}

uint8_t BLOCK_GetSize(void) {
	return block_index;
}

void BLOCK_Clear(void) {
	block_index = 0;
}
