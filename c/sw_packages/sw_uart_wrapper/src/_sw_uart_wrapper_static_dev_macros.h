
#ifndef _sw_uart_wrapper_static_dev_macros_h_
#define _sw_uart_wrapper_static_dev_macros_h_


extern uint8_t sw_uart_wrapper_ioctl(void * const aHandle ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t sw_uart_wrapper_callback(void * const aHandle ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2);
extern size_t sw_uart_wrapper_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);
#include "src/sw_uart_wrapper.h"

#if SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE > 0
	#define __SW_UART_WRAPPER_API_CREATE_STATIC_DEV(dev,dev_name, 		\
				server_dev,client_dev	, rxBuffer,rxBufferSize)		\
				extern const dev_descriptor_t dev ;						\
				extern const dev_descriptor_t server_dev ;				\
				extern const dev_descriptor_t client_dev ;				\
		SW_UART_WRAPPER_Instance_t handle_of_##dev =	 {				\
					&server_dev ,	/*server_dev*/			\
					NULL,			/* xQueue */			\
					1,				/* use_task_for_out */	\
					&dev,			/* this_dev */			\
					&client_dev,	/* client_dev */	\
					rxBuffer,		/* rx_buff */			\
					rxBufferSize,	/* rx_buff_size */		\
					0,				/* WritePos */			\
					0,				/* ReadPos */			\
					0,				/*isDataInUse */		\
					0,				/*bufferWasOverflowed */\
					0,				/* (*sendData) */		\
					0,				/*data_length*/			\
					NULL			/*xTX_WaitQueue*/		\
				};														\
			const dev_descriptor_t dev =								\
				{											\
					dev_name,								\
					&handle_of_##dev,						\
					sw_uart_wrapper_ioctl,					\
					sw_uart_wrapper_pwrite,					\
					DEV_API_dummy_pread_func,				\
					sw_uart_wrapper_callback				\
				}

#else // #if SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE = 0

	#define __SW_UART_WRAPPER_API_CREATE_STATIC_DEV(dev,dev_name, server_dev )	\
			extern const dev_descriptor_t dev ;						\
			extern const dev_descriptor_t server_dev ;				\
			extern const dev_descriptor_t client_dev ;				\
	SW_UART_WRAPPER_Instance_t handle_of_##dev =	 {				\
				&server_dev ,	/*server_dev*/			\
				NULL,			/* xQueue */			\
				1,				/* use_task_for_out */	\
				0,				/* (*sendData) */		\
				0,				/*data_length*/			\
				NULL			/*xTX_WaitQueue*/		\
			};											\
		const dev_descriptor_t dev =					\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				sw_uart_wrapper_ioctl,					\
				sw_uart_wrapper_pwrite,					\
				DEV_API_dummy_pread_func,				\
				sw_uart_wrapper_callback				\
			}


#endif // for    #if SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE > 0




#endif
