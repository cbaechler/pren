

#ifndef APP_H_
#define APP_H_

#include "PE_Types.h"
#include "Event.h"

void APP_Init(void);
void APP_Loop(void);
static void APP_HandleEvent(EVNT_Handle event);

#endif /* APP_H_ */
