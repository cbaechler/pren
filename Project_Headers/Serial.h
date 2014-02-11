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
#define SER_READ_VARIABLE	'r'
#define SER_SAVE_NVM 		's'
#define SER_WRITE_VARIABLE	'w'

typedef enum SER_StateKinds {
	SER_FSM_START, 
	SER_FSM_LENGTH,
	SER_FSM_COMMAND,
	SER_FSM_DATA,
	SER_FSM_CHECKSUM,
	SER_FSM_STOP,
	SER_FSM_BUSY
} SER_StateKinds;

typedef struct SER_Packet {
	uint8_t command;
	uint8_t data[32];
	uint8_t data_index;
	uint8_t length;
	uint8_t checksum;	
} SER_Packet;

typedef struct SER_FSMData {
	SER_StateKinds state;
	SER_Packet input_packet;
	SER_Packet output_packet;
	byte (*ReceiveChar)(uint8_t *ch);
	byte (*SendChar)(uint8_t ch);
} SER_FSMData;

#define SER_GetData8(i)		(SER_GetData()[i])
#define	SER_GetData16(i)	((SER_GetData()[i]<<8)+SER_GetData()[i+1])

void SER_Process(void);
void SER_SetHandled(void);
uint8_t* SER_GetLength(void);
uint8_t* SER_GetCommand(void);
uint8_t* SER_GetData(void);
bool SER_TestChecksum(void);
void SER_AddData8(uint8_t d);
void SER_AddData16(uint16_t d);
uint8_t SER_BuildChecksum(void);
void SER_SendChar(uint8_t ch);
void SER_SendPacket(uint8_t command);

#endif
