
#ifndef _UART_STM8_API_H_
#define _UART_STM8_API_H_


#include "src/_uart_stm8_prerequirements_check.h"
#include "uart_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



/**********  define API  functions  ************/

uint8_t  uart_stm8_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#include "src/_uart_stm8_static_dev_macros.h"


#define UART_STM8_API_CREATE_STATIC_DEV(pdev , callback_pdev) \
			__UART_STM8_API_CREATE_STATIC_DEV(pdev ,  callback_pdev)


#endif
