
#ifndef _UART_ATMEGA_API_H_
#define _UART_ATMEGA_API_H_

#include "uart_atmega_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "uart_api.h"
#include "src/_uart_atmega_prerequirements_check.h" // should be after {uart_atmega_config.h,dev_management_api.h}

/*****************  defines  **************/


/**********  define API  types ************/



/**********  define API  functions  ************/

uint8_t  uart_atmega_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#include "src/_uart_atmega_static_dev_macros.h"

#define UART_ATMEGA_API_CREATE_STATIC_DEV(dev,dev_name , callback_dev) \
			__UART_ATMEGA_API_CREATE_STATIC_DEV(dev,dev_name,  callback_dev)


#endif
