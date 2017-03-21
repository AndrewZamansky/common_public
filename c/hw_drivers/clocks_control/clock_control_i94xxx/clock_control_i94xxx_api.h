
#ifndef _CLOCK_CONTROL_I94XXX_API_H_
#define _CLOCK_CONTROL_I94XXX_API_H_

#include "src/_cortexM_systick_prerequirements_check.h"

/**********  define API  types ************/

typedef enum _clock_control_api_all_clocks {
	I94XXX_XTAL_CLOCK,
	I94XXX_CORE_CLOCK,
	I94XXX_UART0_MODULE_CLOCK,
	I94XXX_TOTAL_NUM_OF_CLOCKS
} clock_control_i94xxx_api_all_clocks_t;

/**********  define API  functions  ************/

#endif
