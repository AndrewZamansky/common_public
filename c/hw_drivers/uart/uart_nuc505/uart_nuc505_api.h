
#ifndef _UART_NUC505_API_H_
#define _UART_NUC505_API_H_

#include "src/_uart_nuc505_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/



/**********  define API  functions  ************/

uint8_t  uart_nuc505_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#include "src/_uart_nuc505_static_dev_macros.h"


#define UART_NUC505_API_CREATE_STATIC_DEV(dev,dev_name ,uart_num, callback_dev) \
			__UART_NUC505_API_CREATE_STATIC_DEV(dev,dev_name, uart_num, callback_dev)


#endif
