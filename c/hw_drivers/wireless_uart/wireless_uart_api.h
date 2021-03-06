
#ifndef _wireless_uart_API_H_
#define _wireless_uart_API_H_

#include "wireless_uart_config.h"
#include "dev_management_api.h"

#define WIRELESS_UART_API_SERVER_DEVICE_STR  	"server_device"


uint8_t  wireless_uart_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);

#include "src/wireless_uart_static_dev_macros.h"

#define UART_WIRELESS_UART_API_CREATE_STATIC_DEV(dev,dev_name, uart_wrapper_server_dev, callback_dev) \
			__UART_WIRELESS_UART_API_CREATE_STATIC_DEV(dev,dev_name, uart_wrapper_server_dev, callback_dev)


#endif
