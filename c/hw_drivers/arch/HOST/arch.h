
#ifndef _ARCH_H_
#define _ARCH_H_

/**********  defines ************/

#ifdef _MSC_VER/* in case we are using microsoft compiler*/


	#define START_OF_MODULES_PLACEMENT	 		const
	#define MODULES_PLACEMENT	 				const
	#define END_OF_MODULES_PLACEMENT	 		const

	#define START_OF_DEVICE_TREE_PLACEMENT	 	const
	#define DEVICE_PLACEMENT	 				const
	#define END_OF_DEVICE_TREE_PLACEMENT	 	const

	#define DEVICE_DATA_PLACEMENT


	#pragma section("init_function_object_section", read)
	#define AUTO_INIT_FUNCTION_PLACEMENT		__declspec(allocate("init_function_object_section"))

#else

	#define START_OF_MODULES_PLACEMENT	 		const
	#define MODULES_PLACEMENT	 				const
	#define END_OF_MODULES_PLACEMENT	 		const

	#define START_OF_DEVICE_TREE_PLACEMENT	 	const
	#define DEVICE_PLACEMENT	 				const
	#define END_OF_DEVICE_TREE_PLACEMENT	 	const

	#define AUTO_INIT_FUNCTION_PLACEMENT	const
	#define DEVICE_DATA_PLACEMENT

#endif


/**********  define   types ************/

/**********  define   functions  ************/



#endif
