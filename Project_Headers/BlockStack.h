/**
 * \file
 * \brief Block Store module header file.
 * \author Christoph Bächler
 * \date 04.01.2014
 */

#include "PE_Types.h"

/*
typedef struct BLOCK_Object {
	uint8_t x;
} BLOCK_Object;*/

typedef uint8_t BLOCK_Object;

#define BLOCK_STACK_MAX_SIZE	10

void BLOCK_Init(void);
bool BLOCK_IsEmtpy(void);
bool BLOCK_IsFull(void);
void BLOCK_Push(BLOCK_Object obj);
BLOCK_Object BLOCK_Pop(void);
BLOCK_Object BLOCK_GetSingle(uint8_t index);
uint8_t BLOCK_GetSize(void);
void BLOCK_Clear(void);
