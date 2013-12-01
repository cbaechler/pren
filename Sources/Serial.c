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
	{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},0,0,0},	// input packet (request)
	{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},0,0,0},	// output packet (answer)
	AS1_RecvChar,
	AS1_SendChar
};

void SER_Init(void) {
	
}

void SER_SendChar(uint8_t ch) {
	while(data.SendChar(ch)==ERR_TXFULL){}
}

void SER_SetHandled(void) {
	data.state = SER_FSM_START;
}

uint8_t* SER_GetLength(void) {
	return &(data.input_packet.length);
}

uint8_t* SER_GetCommand(void) {
	return &(data.input_packet.command);
}

uint8_t* SER_GetData(void) {
	return data.input_packet.data;
}

bool SER_TestChecksum(void) {
	return TRUE; 		// checksum OK!
}

void SER_AddData8(uint8_t d) {
	data.output_packet.data[data.output_packet.data_index] = d;
	data.output_packet.data_index++;
}

void SER_AddData16(uint16_t d) {
	data.output_packet.data[data.output_packet.data_index] = (uint8_t) (d >> 8);
	data.output_packet.data_index++;
	data.output_packet.data[data.output_packet.data_index] = (uint8_t) (d & 0xFF);
	data.output_packet.data_index++;
}

uint8_t SER_BuildChecksum(void) {
	return 0x00;
}

void SER_SendPacket(uint8_t command) {
	uint8_t i;
	data.output_packet.length = data.output_packet.data_index + 5;
	data.output_packet.command = command;
	data.output_packet.checksum = SER_BuildChecksum();
	
	SER_SendChar(SER_START);
	SER_SendChar(data.output_packet.length);
	SER_SendChar(data.output_packet.command);
	
	for(i=0; i<data.output_packet.data_index; i++) {
		SER_SendChar(data.output_packet.data[i]);
	}
	SER_SendChar(data.output_packet.checksum);
	SER_SendChar(SER_END);
}

/* this FSM is called when ever a char arrives on the uart */ 
void SER_Process(void) {
	uint8_t in = 0;
	uint8_t* inp = &in;
	data.ReceiveChar(inp);
	
	switch(data.state) {
		case SER_FSM_START:
			if(*inp == SER_START) {
				data.input_packet.data_index = 0;
				data.output_packet.data_index = 0;
				data.state = SER_FSM_LENGTH;
			}
			else {
				data.state = SER_FSM_START;
			}
			break;

		case SER_FSM_LENGTH:
			data.input_packet.length = *inp;
			data.state = SER_FSM_COMMAND;
			break;
			
		case SER_FSM_COMMAND:
			data.input_packet.command = *inp;
			if(data.input_packet.length > 5) {						// command with data
				data.state = SER_FSM_DATA;
			}
			else {										// command without data
				data.state = SER_FSM_CHECKSUM;
			}
			break;

		case SER_FSM_DATA:
			data.input_packet.data[data.input_packet.data_index] = *inp;
			data.input_packet.data_index++;
			if(data.input_packet.length <= (data.input_packet.data_index + 5)) {
				data.state = SER_FSM_CHECKSUM;
			}
			break;

		case SER_FSM_CHECKSUM:
			data.input_packet.checksum = *inp;
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
