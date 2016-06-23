
#ifndef _UART_STM8_API_H_
#define _UART_STM8_API_H_

#include "uart_stm8_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "uart_api.h"
#include "src/_uart_stm8_prerequirements_check.h" // should be after {uart_stm8_config.h,dev_managment_api.h}

/*****************  defines  **************/


/**********  define API  types ************/



/**********  define API  functions  ************/

uint8_t  uart_stm8_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#include "src/_uart_stm8_static_dev_macros.h"


#define UART_STM8_API_CREATE_STATIC_DEV(dev,dev_name , callback_dev) \
			__UART_STM8_API_CREATE_STATIC_DEV(dev,dev_name,  callback_dev)


#endif
