
#ifndef _CORTEXM_SYSTICK_API_H_
#define _CORTEXM_SYSTICK_API_H_

#include "cortexM_systick_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "hw_timer_api.h"
#include "src/_cortexM_systick_prerequirements_check.h" // should be after {uart_NUC505_config.h,dev_managment_api.h}

/**********  define API  types ************/


/**********  define API  functions  ************/




uint8_t  cortexM_systick_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#include "src/_cortexM_systick_static_dev_macros.h"

#define CORTEXM_SYSTICK_API_CREATE_STATIC_DEV(dev,dev_name , input_clock , rate ,mode , callback) \
		__CORTEXM_SYSTICK_API_CREATE_STATIC_DEV(dev,dev_name ,input_clock , rate , mode , callback)

#endif
