
#ifndef _sw_uart_wrapper_static_dev_macros_h_
#define _sw_uart_wrapper_static_dev_macros_h_


extern uint8_t sw_uart_wrapper_ioctl(void * const aHandle ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t sw_uart_wrapper_callback(void * const aHandle ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2);
extern size_t sw_uart_wrapper_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);
#include "src/sw_uart_wrapper.h"

#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
	#define __SW_UART_WRAPPER_API_CREATE_STATIC_DEV(pdev,		 			\
				server_pdev , client_pdev	, rxBuffer,rxBufferSize)		\
				EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(pdev) ;			\
				EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(server_pdev) ;		\
				EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(client_pdev) ;		\
		SW_UART_WRAPPER_Instance_t handle_of_##pdev =	 {					\
				P_TO_STATIC_DEVICE_INST(server_pdev) ,	/*server_dev*/		\
					NULL,			/* xQueue */							\
					1,				/* use_task_for_out */					\
				P_TO_STATIC_DEVICE_INST(pdev),			/* this_dev */		\
				P_TO_STATIC_DEVICE_INST(client_pdev),	/* client_dev */	\
					rxBuffer,		/* rx_buff */							\
					rxBufferSize,	/* rx_buff_size */						\
					0,				/* WritePos */							\
					0,				/* ReadPos */							\
					0,				/*isDataInUse */						\
					0,				/*bufferWasOverflowed */				\
					0,				/* (*sendData) */						\
					0,				/*data_length*/							\
					NULL			/*xTX_WaitQueue*/						\
				};															\
			STATIC_DEVICE(pdev , &handle_of_##pdev ,sw_uart_wrapper_ioctl ,	\
						sw_uart_wrapper_pwrite , DEV_API_dummy_pread_func , sw_uart_wrapper_callback)

#else // #ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX

	#define __SW_UART_WRAPPER_API_CREATE_STATIC_DEV(pdev , server_pdev )	\
			EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(server_pdev) ;			\
	SW_UART_WRAPPER_Instance_t handle_of_##pdev =	 {						\
			P_TO_STATIC_DEVICE_INST(server_pdev) ,	/*server_dev*/			\
				NULL,			/* xQueue */								\
				1,				/* use_task_for_out */						\
				0,				/* (*sendData) */							\
				0,				/*data_length*/								\
				NULL			/*xTX_WaitQueue*/							\
			};																\
			STATIC_DEVICE(pdev , &handle_of_##pdev ,sw_uart_wrapper_ioctl ,	\
					sw_uart_wrapper_pwrite , DEV_API_dummy_pread_func , sw_uart_wrapper_callback)



#endif // for    #ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX




#endif
