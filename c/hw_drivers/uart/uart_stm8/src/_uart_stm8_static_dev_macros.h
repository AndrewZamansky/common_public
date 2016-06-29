
#ifndef _uart_stm8_static_dev_macros_h_
#define _uart_stm8_static_dev_macros_h_

#include "src/uart_stm8.h"
uint8_t uart_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t uart_stm8_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);


#define __UART_STM8_API_CREATE_STATIC_DEV(pdev ,  callback_pdev)			\
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(callback_pdev) ;			\
		UART_STM8_Instance_t handle_of_##pdev =	 							\
				{P_TO_STATIC_DEVICE_INST(callback_pdev),115200};			\
		STATIC_DEVICE(pdev , &handle_of_##pdev , uart_stm8_ioctl , 			\
				uart_stm8_pwrite , DEV_API_dummy_pread_func , DEV_API_dummy_callback_func)

#endif
