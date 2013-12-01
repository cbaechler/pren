/**
 * \file
 * \brief Serial Module implementation.
 * \author Christoph Bächler
 * \date 12.11.2013
 */

#include "PE_Types.h"
#include "Event.h"
#include "Serial.h"
#include "AS1.h"

static SER_FSMData data = {
	SER_FSM_START,
	0,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},		// equivalent to buffer size (32)
	0,
	0,
	0,
	AS1_RecvChar,
	AS1_SendChar
};

void SER_Init(void) {
	
}

void SER_ReceiveChar(void) {
	// for now just echo
	uint8_t in = 0;
	uint8_t* inp = &in;
	AS1_RecvChar(inp);
	AS1_SendChar(*inp);
}

void SER_SetHandled(void) {
	data.state = SER_FSM_START;
}

uint8_t* SER_GetLength(void) {
	return &(data.length);
}

uint8_t* SER_GetCommand(void) {
	return &(data.command);
}

uint8_t* SER_GetData(void) {
	return data.data;
}

bool SER_TestChecksum(void) {
	return TRUE; 		// checksum OK!
}

/* this FSM is called when ever a char arrives on the uart */ 
void SER_Process(void) {
	uint8_t in = 0;
	uint8_t* inp = &in;
	data.ReceiveChar(inp);
	
	switch(data.state) {
		case SER_FSM_START:
			if(*inp == SER_START) {
				data.data_index = 0;
				data.state = SER_FSM_LENGTH;
			}
			else {
				data.state = SER_FSM_START;
			}
			break;

		case SER_FSM_LENGTH:
			data.length = *inp;
			data.state = SER_FSM_COMMAND;
			break;
			
		case SER_FSM_COMMAND:
			data.command = *inp;
			if(data.length > 5) {						// command with data
				data.state = SER_FSM_DATA;
			}
			else {										// command without data
				data.state = SER_FSM_CHECKSUM;
			}
			break;

		case SER_FSM_DATA:
			data.data[data.data_index] = *inp;
			data.data_index++;
			if(data.length <= (data.data_index + 5)) {
				data.state = SER_FSM_CHECKSUM;
			}
			break;

		case SER_FSM_CHECKSUM:
			data.checksum = *inp;
			data.state = SER_FSM_STOP;
			break;

		case SER_FSM_STOP:
			if(*inp == SER_END) {
				EVNT_SetEvent(EVNT_SERIAL_CMD);
				data.state = SER_FSM_BUSY;
			}
			else {
				data.state = SER_FSM_START;
			}
			break;

		case SER_FSM_BUSY:
			// while packet is not handled, fsm will stay in this state...
			break;

		default: 
			data.state = SER_FSM_START;
			break;
	}
}
