
#ifndef _CLOCK_CONTROL_NUC505_API_H_
#define _CLOCK_CONTROL_NUC505_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

/**********  define API  types ************/


/**********  define API  functions  ************/

extern struct dev_desc_t  *nuc505_xtal_clk_dev;
extern struct dev_desc_t  *nuc505_core_clk_dev;
extern struct dev_desc_t  *nuc505_systick_clk_dev;

#endif
