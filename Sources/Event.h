/**
 * \file
 * \brief Event Module header file.
 * \author Christoph Bächler
 * \date 11.11.2013
 *
 * This module implements a generic event driver. We are using numbered events starting with zero.
 * EVNT_HandleEvent() can be used to process the pending events. Note that the event with the number zero
 * has the highest priority and will be handled first.
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "PE_Types.h"

typedef enum EVNT_Handle {
	EVNT_INIT,						/*!< System Initialisation Event */
	EVNT_HEARTBEAT,
	EVNT_NOF_EVENTS					/*!< Sentinel only, must be last one */

} EVNT_Handle;


/*! \brief Event module initialization */
void EVNT_Init(void);

/*!
 * \brief Sets an event.
 * \param[in] event The handle of the event to set.
 */
void EVNT_SetEvent(EVNT_Handle event);

/*!
 * \brief Clears an event.
 * \param[in] event The event handle of the event to clear.
 */
void EVNT_ClearEvent(EVNT_Handle event);

/*!
 * \brief Returns the status of an event.
 * \param[in] event The event handler of the event to check.
 * \return TRUE if the event is set, FALSE otherwise.
 */
bool EVNT_EventIsSet(EVNT_Handle event);

/*!
 * \brief Routine to check if an event is pending. If an event is pending, the event is cleared and the provided callback is called.
 * \param[in] callback Callback routine to be called. The event handle is passed as argument to the callback.
 */
void EVNT_HandleEvent(void (*callback)(EVNT_Handle));


#endif /* EVENT_H_ */
