/* ###################################################################
**     Filename    : ProcessorExpert.c
**     Project     : ProcessorExpert
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2013-11-11, 19:57, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file ProcessorExpert.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup ProcessorExpert_module ProcessorExpert module documentation
**  @{
*/         
/* MODULE ProcessorExpert */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "LED_RED.h"
#include "LEDpin2.h"
#include "BitIoLdd1.h"
#include "LED_GREEN.h"
#include "LEDpin1.h"
#include "BitIoLdd2.h"
#include "LED_BLUE.h"
#include "LEDpin3.h"
#include "BitIoLdd3.h"
#include "WAIT1.h"
#include "AS1.h"
#include "ASerialLdd1.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "SIG.h"
#include "M1_STEP.h"
#include "BitIoLdd4.h"
#include "M2_STEP.h"
#include "BitIoLdd5.h"
#include "M3_STEP.h"
#include "BitIoLdd6.h"
#include "ILIM.h"
#include "M1_DIR.h"
#include "BitIoLdd7.h"
#include "M1_nRST.h"
#include "BitIoLdd8.h"
#include "M1_MODE0.h"
#include "BitIoLdd9.h"
#include "M1_MODE1.h"
#include "BitIoLdd10.h"
#include "M1_MODE2.h"
#include "BitIoLdd11.h"
#include "M1_FAULT.h"
#include "BitIoLdd12.h"
#include "M2_DIR.h"
#include "BitIoLdd13.h"
#include "M2_nRST.h"
#include "BitIoLdd14.h"
#include "M2_MODE0.h"
#include "BitIoLdd15.h"
#include "M2_MODE1.h"
#include "BitIoLdd16.h"
#include "M2_MODE2.h"
#include "BitIoLdd17.h"
#include "M2_FAULT.h"
#include "BitIoLdd18.h"
#include "M3_DIR.h"
#include "BitIoLdd19.h"
#include "M3_nRST.h"
#include "BitIoLdd20.h"
#include "M3_MODE0.h"
#include "BitIoLdd21.h"
#include "M3_MODE1.h"
#include "BitIoLdd22.h"
#include "M3_MODE2.h"
#include "BitIoLdd23.h"
#include "M3_FAULT.h"
#include "BitIoLdd24.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Application.h"

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  APP_Init();
  APP_Loop();	/* should not return */
  /*
  while(1) {
	  LED_GREEN_Put(1);
	  WAIT1_Waitms(1000);
	  LED_GREEN_Put(0);
	  WAIT1_Waitms(1000);
	  AS1_SendChar('a');
  }*/
  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END ProcessorExpert */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.08]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
