
#ifndef _UART_NUC505_API_H_
#define _UART_NUC505_API_H_

#include "src/_uart_nuc505_prerequirements_check.h"

/*****************  defines  **************/

#define UART_NUC505_API_UART_ID_0	0
#define UART_NUC505_API_UART_ID_1	1
#define UART_NUC505_API_UART_ID_2	2

/**********  define API  types ************/



/**********  define API  functions  ************/

uint8_t  uart_nuc505_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#endif
