
#ifndef _STM8_API_H_
#define _STM8_API_H_

/**********  defines ************/



#define arch_enter_interrupt()		 atomIntEnter();\
									_asm("ldw Y,SP\n ldw X,#0x3ff\n  ldw SP,X\n PUSHW Y\n")

#define arch_exit_interrupt(forceResceduling)		_asm(" POPW Y \n ldw SP,Y\n ");\
													atomIntExit(forceResceduling)

/**********  define API  types ************/

/**********  define API  functions  ************/


#endif
