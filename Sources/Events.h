/* ###################################################################
**     Filename    : Events.h
**     Project     : ProcessorExpert
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2014-01-26, 23:16, # CodeGen: 78
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         SIG_OnCounterRestart - void SIG_OnCounterRestart(LDD_TUserData *UserDataPtr);
**         SIG_OnChannel0       - void SIG_OnChannel0(LDD_TUserData *UserDataPtr);
**         SIG_OnChannel1       - void SIG_OnChannel1(LDD_TUserData *UserDataPtr);
**         SIG_OnChannel2       - void SIG_OnChannel2(LDD_TUserData *UserDataPtr);
**         SYS_TICK_OnInterrupt - void SYS_TICK_OnInterrupt(void);
**         DBG_OnError          - void DBG_OnError(void);
**         DBG_OnRxChar         - void DBG_OnRxChar(void);
**         DBG_OnTxChar         - void DBG_OnTxChar(void);
**         Cpu_OnNMIINT         - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "LED_RED.h"
#include "LEDpin2.h"
#include "BitIoLdd1.h"
#include "LED_GREEN.h"
#include "LEDpin1.h"
#include "BitIoLdd2.h"
#include "LED_BLUE.h"
#include "LEDpin3.h"
#include "BitIoLdd3.h"
#include "WAIT.h"
#include "DBG.h"
#include "ASerialLdd1.h"
#include "SYS_TICK.h"
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
#include "BT.h"
#include "Serial1.h"
#include "ASerialLdd2.h"
#include "UTIL.h"
#include "NVM.h"
#include "IntFlashLdd1.h"

#ifdef __cplusplus
extern "C" {
#endif 

/*
** ===================================================================
**     Event       :  SIG_OnCounterRestart (module Events)
**
**     Component   :  SIG [TimerUnit_LDD]
*/
/*!
**     @brief
**         Called if counter overflow/underflow or counter is
**         reinitialized by modulo or compare register matching.
**         OnCounterRestart event and Timer unit must be enabled. See
**         [SetEventMask] and [GetEventMask] methods. This event is
**         available only if a [Interrupt] is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer passed as
**                           the parameter of Init method.
*/
/* ===================================================================*/
void SIG_OnCounterRestart(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Event       :  SIG_OnChannel0 (module Events)
**
**     Component   :  SIG [TimerUnit_LDD]
*/
/*!
**     @brief
**         Called if compare register match the counter registers or
**         capture register has a new content. OnChannel0 event and
**         Timer unit must be enabled. See [SetEventMask] and
**         [GetEventMask] methods. This event is available only if a
**         [Interrupt] is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer passed as
**                           the parameter of Init method.
*/
/* ===================================================================*/
void SIG_OnChannel0(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Event       :  SIG_OnChannel1 (module Events)
**
**     Component   :  SIG [TimerUnit_LDD]
*/
/*!
**     @brief
**         Called if compare register match the counter registers or
**         capture register has a new content. OnChannel1 event and
**         Timer unit must be enabled. See [SetEventMask] and
**         [GetEventMask] methods. This event is available only if a
**         [Interrupt] is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer passed as
**                           the parameter of Init method.
*/
/* ===================================================================*/
void SIG_OnChannel1(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Event       :  SIG_OnChannel2 (module Events)
**
**     Component   :  SIG [TimerUnit_LDD]
*/
/*!
**     @brief
**         Called if compare register match the counter registers or
**         capture register has a new content. OnChannel2 event and
**         Timer unit must be enabled. See [SetEventMask] and
**         [GetEventMask] methods. This event is available only if a
**         [Interrupt] is enabled.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer passed as
**                           the parameter of Init method.
*/
/* ===================================================================*/
void SIG_OnChannel2(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Event       :  SYS_TICK_OnInterrupt (module Events)
**
**     Component   :  SYS_TICK [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SYS_TICK_OnInterrupt(void);

/*
** ===================================================================
**     Event       :  DBG_OnError (module Events)
**
**     Component   :  DBG [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void DBG_OnError(void);

/*
** ===================================================================
**     Event       :  DBG_OnRxChar (module Events)
**
**     Component   :  DBG [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void DBG_OnRxChar(void);

/*
** ===================================================================
**     Event       :  DBG_OnTxChar (module Events)
**
**     Component   :  DBG [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void DBG_OnTxChar(void);

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void);


/*
** ===================================================================
**     Event       :  Serial1_OnRxChar (module Events)
**
**     Component   :  Serial1 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Serial1_OnRxChar(void);

/*
** ===================================================================
**     Event       :  NVM_OnWriteEnd (module Events)
**
**     Component   :  NVM [IntFLASH]
*/
/*!
**     @brief
**         Event is called after a write operation to FLASH memory is
**         finished (except [SetPage]). This event is available only if
**         an [Interrupt service/event] is selected.
*/
/* ===================================================================*/
void NVM_OnWriteEnd(void);

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __Events_H*/
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
