
#ifndef _clocks_control_stm32f10x_API_H_
#define _clocks_control_stm32f10x_API_H_

/*****************  defines  **************/

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

/**********  define API  types ************/




/**********  define API  functions  ************/
uint32_t  clocks_control_stm32f10x_init(void);
uint32_t clocks_control_stm32f10x_get_cpu_clock(void );

#endif
