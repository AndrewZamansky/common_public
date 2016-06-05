
#ifndef _CORTEXM_SYSTICK_API_H_
#define _CORTEXM_SYSTICK_API_H_

#include "src/_cortexM_systick_prerequirements_check.h"

/**********  define API  types ************/


/**********  define API  functions  ************/




uint8_t  cortexM_systick_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#include "src/_cortexM_systick_static_dev_macros.h"

#define CORTEXM_SYSTICK_API_CREATE_STATIC_DEV(dev,dev_name ,  rate ,mode , callback) \
		__CORTEXM_SYSTICK_API_CREATE_STATIC_DEV(dev,dev_name ,  rate , mode , callback)

#endif
