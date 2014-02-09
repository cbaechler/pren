/**
 * \file
 * \brief Block Store module header file.
 * \author Christoph Bächler
 * \date 04.01.2014
 */

#include "PE_Types.h"

typedef struct BLOCK_Object {
	uint16_t x;
	uint16_t y;
} BLOCK_Object;

typedef enum BLOCK_StateKinds {
	BLOCK_IDLE,
	BLOCK_NEXT,
	BLOCK_PICK,
	BLOCK_PICKED,
	BLOCK_RELEASE,
	BLOCK_RELEASED
} BLOCK_StateKinds;

typedef struct BLOCK_FSMData {
	BLOCK_StateKinds state;
	bool started;
} BLOCK_FSMData;

#define BLOCK_STACK_MAX_SIZE	10

void BLOCK_Init(void);
void BLOCK_StartPickPlace(void);
uint8_t BLOCK_PickPlace_GetState(void);
void BLOCK_PickPlace_Process(void);
bool BLOCK_IsEmtpy(void);
bool BLOCK_IsFull(void);
void BLOCK_Push(BLOCK_Object obj);
BLOCK_Object BLOCK_Pop(void);
BLOCK_Object BLOCK_GetSingle(uint8_t index);
uint8_t BLOCK_GetSize(void);
void BLOCK_Clear(void);
