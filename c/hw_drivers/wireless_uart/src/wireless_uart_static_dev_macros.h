
#ifndef _wireless_uart_static_dev_macros_h_
#define _wireless_uart_static_dev_macros_h_

#include "wireless_uart_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_wireless_uart_prerequirements_check.h" // should be after {wireless_uart_config.h,dev_managment_api.h}

extern uint8_t wireless_uart_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1  , void * aIoctl_param2);
extern uint8_t wireless_uart_callback(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);
size_t wireless_uart_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);

#include "src/wireless_uart.h"

#define __UART_WIRELESS_UART_API_CREATE_STATIC_DEV(dev,dev_name, uart_wrapper_server_dev, callback_dev)\
		extern const dev_descriptor_t dev ;						\
		extern const dev_descriptor_t uart_wrapper_server_dev ;			\
		extern const dev_descriptor_t callback_dev ;			\
		UART_WIRELESS_UART_Instance_t handle_of_##dev =	 {&uart_wrapper_server_dev,&callback_dev,&dev};	\
		const dev_descriptor_t dev =								\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				wireless_uart_ioctl,					\
				wireless_uart_pwrite,					\
				DEV_API_dummy_pread_func,				\
				wireless_uart_callback					\
			}


#endif
