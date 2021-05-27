
#ifndef _uart_atmega_static_dev_macros_h_
#define _uart_atmega_static_dev_macros_h_

#include "src/uart_atmega.h"
uint8_t uart_atmega_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void* aIoctl_param1,void* aIoctl_param2);
size_t uart_atmega_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);


#define __UART_ATMEGA_API_CREATE_STATIC_DEV(dev,dev_name,  callback_dev)\
		extern const struct dev_desc_t dev ;           \
		extern const struct dev_desc_t callback_dev ;  \
		struct UART_ATMEGA_Instance_t handle_of_##dev = {&callback_dev,115200}; \
		const struct dev_desc_t dev =  \
			{                                   \
				dev_name,                     \
				&handle_of_##dev,             \
				uart_atmega_ioctl,            \
				uart_atmega_pwrite,           \
				DEV_API_dummy_pread_func,     \
				DEV_API_dummy_callback_func   \
			}

#endif
