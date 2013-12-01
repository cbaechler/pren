/**
 * \file
 * \brief Interface for Serial Module.
 * \author Christoph Baechler
 * \date 28.11.2013
 */

#ifndef SERIAL_H_
#define SERIAL_H_

/* Define protocol */
#define SER_START 			'['
#define SER_END				']'

#define SER_MODE			'M'
#define SER_BLOCK_ARRAY		'A'
#define SER_RUN				'R'
#define SER_PICK_BLOCK		'P'
#define SER_RELEASE_BLOCK	'Q'
#define SER_SET_POSITION	'S'
#define SER_GET_POSITION	'T'
#define SER_GET_VERSION		'V'
#define SER_ZERO_POSITION	'Z'

typedef enum SER_StateKinds {
	SER_FSM_START, 
	SER_FSM_LENGTH,
	SER_FSM_COMMAND,
	SER_FSM_DATA,
	SER_FSM_CHECKSUM,
	SER_FSM_STOP,
	SER_FSM_BUSY
} SER_StateKinds;

typedef struct SER_FSMData {
	SER_StateKinds state;
	uint8_t command;
	uint8_t data[32];
	uint8_t data_index;
	uint8_t length;
	uint8_t checksum;
	byte (*ReceiveChar)(uint8_t *ch);
	byte (*SendChar)(uint8_t ch);
} SER_FSMData;

void SER_Process(void);
void SER_SetHandled(void);
uint8_t* SER_GetLength(void);
uint8_t* SER_GetCommand(void);
uint8_t* SER_GetData(void);
bool SER_TestChecksum(void);

#endif
