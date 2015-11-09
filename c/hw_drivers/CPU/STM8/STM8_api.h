
#ifndef _STM8_API_H_
#define _STM8_API_H_

/**********  defines ************/

/* compiler dependent definitions : */
#if defined(__CSMC__)
#undef INTERRUPT
#define INTERRUPT @far @interrupt @svlreg
#elif defined (__IAR_SYSTEMS_ICC__)
#define INTERRUPT __interrupt
#elif defined(__RCSTM8__)
#define INTERRUPT
#endif


#define arch_enter_interrupt()		 atomIntEnter();\
									_asm("ldw Y,SP\n ldw X,#0x3ff\n  ldw SP,X\n PUSHW Y\n")

#define arch_exit_interrupt(forceResceduling)		_asm(" POPW Y \n ldw SP,Y\n ");\
													atomIntExit(forceResceduling)

/**********  define API  types ************/

/**********  define API  functions  ************/


#endif
