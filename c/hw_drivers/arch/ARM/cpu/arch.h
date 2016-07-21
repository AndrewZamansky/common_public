
#ifndef _ARCH_H_
#define _ARCH_H_

/**********  defines ************/


#ifdef CONFIG_GCC
	#define DEVICE_PLACEMENT	 				__attribute__((section(".static_devs")))
	#define DEVICE_DATA_PLACEMENT	 			__attribute__((section(".static_devs_data")))
	#define AUTO_INIT_FUNCTION_PLACEMENT	 	__attribute__((section(".auto_init_functions")))
#endif



/**********  define   types ************/

/**********  define   functions  ************/



#endif
