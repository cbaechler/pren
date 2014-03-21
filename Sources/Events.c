/* ###################################################################
**     Filename    : Events.c
**     Project     : ProcessorExpert
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2013-11-11, 19:57, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Event.h"
#include "Serial.h"
#include "Timer.h"
#include "SIG.h"
#include "LED_RED.h"
#include "Motors.h"
#include "M1_STEP.h"
#include "M2_STEP.h"

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
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

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
void DBG_OnError(void)
{
  /* Write your code here ... */
}

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
void DBG_OnRxChar(void)
{
	SER_Process();
}

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
void DBG_OnTxChar(void)
{
  /* Write your code here ... */
}

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
void SYS_TICK_OnInterrupt(void)
{
	TMR_OnInterrupt();
}

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
void SIG_OnChannel0(LDD_TUserData *UserDataPtr)
{
	if(rotary.running == TRUE) {
		TPM0_C0V = TPM0_CNT + MOT_Process(&rotary);
		M1_STEP_NegVal();
		//LED_RED_Neg();
	}
}

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
void SIG_OnChannel1(LDD_TUserData *UserDataPtr)
{
	if(knee.running == TRUE) {
		TPM0_C1V = TPM0_CNT + MOT_Process(&knee);
		M2_STEP_NegVal();
		//LED_GREEN_Neg();
	}
}

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
void SIG_OnChannel2(LDD_TUserData *UserDataPtr)
{
	if(lift.running == TRUE) {
		TPM0_C2V = TPM0_CNT + MOT_Process(&lift);
		M3_STEP_NegVal();
	}
}

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
void SIG_OnCounterRestart(LDD_TUserData *UserDataPtr)
{
  /* Write your code here ... */
}

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
void Serial1_OnRxChar(void)
{
	SER_Process();
  /* Write your code here ... */
}

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
void NVM_OnWriteEnd(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  DBG_OnFullRxBuf (module Events)
**
**     Component   :  DBG [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void DBG_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  DBG_OnFreeTxBuf (module Events)
**
**     Component   :  DBG [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void DBG_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
