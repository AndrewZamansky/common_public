
#ifndef _USB_Virtual_COM_API_H_
#define _USB_Virtual_COM_API_H_

#include "uart_api.h"

/*****************  defines  **************/


/**********  define API  types ************/




/**********  define API  functions  ************/

typedef enum
{
	IOCTL_USB_VIRTUAL_COM_START = IOCTL_LAST_COMMON_UART_IOCTL + 1,
} USB_VIRTUAL_COM_API_ioctl_t;


#endif
