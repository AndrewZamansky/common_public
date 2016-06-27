
#ifndef _auto_init_api_h_
#define _auto_init_api_h_


#include "src/_auto_init_prerequirements_check.h"
#include "arch.h"

/**********  defines ************/

#define NAGIC_NUMBER	0x1A3C
typedef void (*auto_init_func_t)(void)  ;

typedef struct
{
	uint16_t 			magic_number;
	auto_init_func_t 	auto_init_func;
}auto_init_struct_t;

#define AUTO_INIT_FUNCTION(func)  \
		AUTO_INIT_FUNCTION_PLACEMENT auto_init_struct_t auto_init_##func = { NAGIC_NUMBER, func}


/**********  define API  types ************/


/**********  define API  functions  ************/

void auto_init_api(void);


#endif
