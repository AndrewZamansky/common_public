
#ifndef _auto_init_api_h_
#define _auto_init_api_h_


#include "src/_auto_init_prerequirements_check.h"
#include "arch.h"

/**********  defines ************/

typedef void (*auto_init_func_t)(void)  ;

#define AUTO_INIT_FUNCTION(func)  \
		AUTO_INIT_FUNCTION_PLACEMENT auto_init_func_t auto_init_##func = func


/**********  define API  types ************/


/**********  define API  functions  ************/

void auto_init_api(void);


#endif
