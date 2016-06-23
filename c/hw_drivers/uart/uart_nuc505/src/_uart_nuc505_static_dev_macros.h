
#ifndef _uart_nuc505_static_dev_macros_h_
#define _uart_nuc505_static_dev_macros_h_

#include "src/uart_nuc505.h"
uint8_t uart_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t uart_nuc505_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);


#define __UART_NUC505_API_CREATE_STATIC_DEV( pdev , uart_num, callback_pdev , baud_rate )	\
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(callback_pdev) ;							\
		UART_NUC505_Instance_t handle_of_##pdev = 											\
				{P_TO_STATIC_DEVICE_INST(callback_pdev) , uart_num , baud_rate};			\
		STATIC_DEVICE(pdev , &handle_of_##pdev , uart_nuc505_ioctl , 						\
				uart_nuc505_pwrite , DEV_API_dummy_pread_func , DEV_API_dummy_callback_func)


#endif
