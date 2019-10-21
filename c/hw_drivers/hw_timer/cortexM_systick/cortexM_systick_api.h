
#ifndef _CORTEXM_SYSTICK_API_H_
#define _CORTEXM_SYSTICK_API_H_

#include "dev_management_api.h"
#include "hw_timer_api.h"


struct cortexM_systick_instance_t {
	uint32_t rate;
	struct dev_desc_t *   clock_dev;
	timer_callback_func_t timer_callback;
	uint8_t mode;
};




SET_CONFIG_TYPE(cortexM_systick, struct cortexM_systick_instance_t);

#endif
