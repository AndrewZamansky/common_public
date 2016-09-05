
#ifndef _ARCH_H_
#define _ARCH_H_

/**********  defines ************/


#ifdef CONFIG_GCC
	#define START_OF_MODULES_PLACEMENT	 		__attribute__((section(".start_of_modules")))
	#define MODULES_PLACEMENT	 				__attribute__((section(".modules")))
	#define END_OF_MODULES_PLACEMENT	 		__attribute__((section(".end_of_modules")))

	#define START_OF_DEVICE_TREE_PLACEMENT	 	__attribute__((section(".start_of_device_tree")))
	#define DEVICE_PLACEMENT	 				__attribute__((section(".static_devs")))
	#define END_OF_DEVICE_TREE_PLACEMENT	 	__attribute__((section(".end_of_device_tree")))

	#define DEVICE_DATA_PLACEMENT	 			__attribute__((section(".static_devs_data")))
	#define AUTO_INIT_FUNCTION_PLACEMENT	 	__attribute__((section(".auto_init_functions")))
#endif



/**********  define   types ************/

/**********  define   functions  ************/



#endif
