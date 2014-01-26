/**
 * \file
 * \brief Serial Module implementation.
 * \author Christoph Bächler
 * \date 12.11.2013
 *
 * This module implements the serial protocol as described in the PREN1 design 
 * concept documentation. Core of the module is the SER_Process() function 
 * which implements a finite state machine to read the packets char-by-char 
 * from the serial interface. 
 */

#include "PE_Types.h"
#include "Event.h"
#include "Serial.h"
#include "DBG.h"

static SER_FSMData data = {
	SER_FSM_START,
	{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},0,0,0},	// input packet (request)
	{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},0,0,0},	// output packet (answer)
	DBG_RecvChar,
	DBG_SendChar
};

void SER_Init(void) {
	
}

/*! \brief Sends a single char to the uart.
 *
 *  \param ch  Char to send
 */
void SER_SendChar(uint8_t ch) {
	while(data.SendChar(ch)==ERR_TXFULL){}
}

/*! \brief Sets the packet to handled state.
 */
void SER_SetHandled(void) {
	data.state = SER_FSM_START;
}

/*! \brief Returns the length of the packet.
 *
 *  \return Length of the packet
 */
uint8_t* SER_GetLength(void) {
	return &(data.input_packet.length);
}

/*! \brief Returns the command byte of the packet.
 *
 *  \return Command byte of the packet
 */
uint8_t* SER_GetCommand(void) {
	return &(data.input_packet.command);
}

/*! \brief Returns the data bytes of the packet.
 *	Since this is a pointer to the first data byte in the packet 
 *  further bytes can be read by indexing: SER_GetData()[7]
 *  \return Data bytes of the packet
 */
uint8_t* SER_GetData(void) {
	return data.input_packet.data;
}

/*! \brief Returns if checksum is correct.
 *
 *  \return TRUE = checksum ok
 */
bool SER_TestChecksum(void) {
	// TODO: implement test of the checksum
	return TRUE;
}

/*! \brief Adds a byte (uint8_t) to the packet.
 *
 *  \param d  Data byte to add
 */
void SER_AddData8(uint8_t d) {
	data.output_packet.data[data.output_packet.data_index] = d;
	data.output_packet.data_index++;
}

/*! \brief Adds two bytes (uint16_t) to the packet.
 *
 *  \param d  Data bytes to add
 */
void SER_AddData16(uint16_t d) {
	data.output_packet.data[data.output_packet.data_index] = (uint8_t) (d >> 8);
	data.output_packet.data_index++;
	data.output_packet.data[data.output_packet.data_index] = (uint8_t) (d & 0xFF);
	data.output_packet.data_index++;
}

/*! \brief Calculates and returns the checksum of the packet.
 *
 *  \return Checksum of the packet
 */
uint8_t SER_BuildChecksum(void) {
	// TODO: implement
	return 0x00;
}

/*! \brief Sends a packet to the serial line.
 *
 *  \param command  Command code of the packet
 */
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
	
	data.output_packet.data_index = 0;
}

/*! \brief FSM to receive packets.
 *
 *  This function is called from interrupt after a byte has arrived.
 */
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
			if(data.input_packet.length > 5) {			// command with data
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
			// while packet is not handled, fsm will stay in this state until packet is handled...
			break;

		default: 
			data.state = SER_FSM_START;
			break;
	}
}
