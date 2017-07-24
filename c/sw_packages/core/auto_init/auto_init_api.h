
#ifndef _auto_init_api_h_
#define _auto_init_api_h_


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "arch.h"

/**********  defines ************/

#ifdef _MSC_VER/* in case we are using microsoft compiler*/
	/* $a will be begining $c will be end of auto init functions*/
	#pragma section("init_function_object_section$b", read)
	#define AUTO_INIT_FUNCTION_PLACEMENT	 				\
				__declspec(dllexport) __declspec(align(8)) 	\
				__declspec(allocate("init_function_object_section$b"))
#elif defined(CONFIG_HEXAGON_COMPILER) || defined(CONFIG_ANDROID_NDK) || \
	 defined(CONFIG_XTENSA_GCC)

	#define AUTO_INIT_FUNCTION_PLACEMENT	 	\
							__attribute__((section("auto_init_section")))
#elif defined(CONFIG_XTENSA_XCC)
	#define AUTO_INIT_FUNCTION_PLACEMENT	 	\
							const __attribute__((section(".rodata.autoinit")))
#endif


#define AUTO_INIT_MAGIC_NUMBER	0x1A3C
typedef void (*auto_init_func_t)(void)  ;

typedef struct
{
	int 				*magic_number;/* define field of pointer size */
	auto_init_func_t 	auto_init_func;
}auto_init_struct_t;

#define AUTO_INIT_FUNCTION(func)  						\
		auto_init_struct_t AUTO_INIT_FUNCTION_PLACEMENT \
		auto_init_##func = { (int*)AUTO_INIT_MAGIC_NUMBER , func}


/**********  define API  types ************/


/**********  define API  functions  ************/

void auto_init_api(void);


#endif
