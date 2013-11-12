/**
 * \file
 * \brief Timer Module implementation.
 * \author Christoph Bächler
 * \date 12.11.2013
 */

#include "Timer.h"
#include "Event.h"
#include "Trigger.h"

void TMR_OnInterrupt(void) {
	/* disable heartbeat
	static uint16_t cnt = 0;	
	cnt++;
	if(cnt==1000/TMR_TICK_MS) { 		// every 1 second
		EVNT_SetEvent(EVNT_HEARTBEAT);
		cnt = 0;
	}
	*/

	TRG_IncTick();
}
