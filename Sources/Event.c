/**
 * \file
 * \brief Event Module.
 * \author Christoph Bächler
 * \date 11.11.2013
 *
 * This module contains event structure.
 */

#include "PE_Types.h"
#include "Cpu.h"
#include "Event.h"

typedef uint8_t EVNT_MemUnit;
#define EVNT_MEM_UNIT_NOF_BITS	(sizeof(EVNT_MemUnit)*8)
static EVNT_MemUnit EVNT_Events[((EVNT_NOF_EVENTS-1)/EVNT_MEM_UNIT_NOF_BITS)+1];

void EVNT_Init(void) {
	uint8_t i;

	i = 0;
	do {
		EVNT_Events[i] = 0;
		i++;
	} while(i<sizeof(EVNT_Events)/sizeof(EVNT_Events[0]));
}

void EVNT_SetEvent(EVNT_Handle event) {
	EnterCritical();
	EVNT_Events[event/8] |= 0x80>>(event%8);
	ExitCritical();
}

void EVNT_ClearEvent(EVNT_Handle event) {
	EnterCritical();
	EVNT_Events[event/8] &= ~(0x80>>(event%8));
	ExitCritical();
}

bool EVNT_EventIsSet(EVNT_Handle event) {
	bool result;
	EnterCritical();
	result = EVNT_Events[event/8] & (0x80>>(event%8));
	ExitCritical();
	return result;
}

void EVNT_HandleEvent(void (*callback)(EVNT_Handle)) {
  /* Handle the one with the highest priority. Zero is the event with the highest priority. */
   EVNT_Handle event;

   EnterCritical();
   for (event=(EVNT_Handle)0; event<EVNT_NOF_EVENTS; event++) { /* does a test on every event */
     if (EVNT_EventIsSet(event)) { /* event present? */
       EVNT_ClearEvent(event); /* clear event */
       break; /* get out of loop */
     }
   }
   ExitCritical();
   if (event != EVNT_NOF_EVENTS) {
     callback(event);
     /* Note: if the callback sets the event, we will get out of the loop.
      * We will catch it by the next iteration.
      */
   }
}
