
#ifndef _ARCH_H_
#define _ARCH_H_

/**********  defines ************/

#define START_OF_MODULES_PLACEMENT	 		const
#define MODULES_PLACEMENT	 				const
#define END_OF_MODULES_PLACEMENT	 		const

#define START_OF_DEVICE_TREE_PLACEMENT	 	const
#define DEVICE_PLACEMENT	 				const
#define END_OF_DEVICE_TREE_PLACEMENT	 	const

#define AUTO_INIT_FUNCTION_PLACEMENT	const
#define DEVICE_DATA_PLACEMENT


#define arch_enter_interrupt()		 atomIntEnter();\
									_asm("ldw Y,SP\n ldw X,#0x3ff\n  ldw SP,X\n PUSHW Y\n")

#define arch_exit_interrupt(forceResceduling)		_asm(" POPW Y \n ldw SP,Y\n ");\
													atomIntExit(forceResceduling)

/**********  define   types ************/

/**********  define   functions  ************/



#endif
