#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "sw_uart_wrapper_api.h"

#undef _add_static_device_step2

#undef SW_UART_WRAPPER_DT_DEV_NAME
#undef SW_UART_WRAPPER_DT_SERVER_PDEV
#undef SW_UART_WRAPPER_DT_CLIENT_PDEV
#undef SW_UART_WRAPPER_DT_RX_BUFFER
#undef SW_UART_WRAPPER_DT_RX_BUFFER_SIZE


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef SW_UART_WRAPPER_DT_DEV_NAME
#error "SW_UART_WRAPPER_DT_DEV_NAME should be defined"
#endif

#ifndef SW_UART_WRAPPER_DT_SERVER_PDEV
#error "SW_UART_WRAPPER_DT_SERVER_PDEV should be defined"
#endif

#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX

#ifndef SW_UART_WRAPPER_DT_CLIENT_PDEV
#error "SW_UART_WRAPPER_DT_CLIENT_PDEV should be defined"
#endif

#ifndef SW_UART_WRAPPER_DT_RX_BUFFER
#error "SW_UART_WRAPPER_DT_RX_BUFFER should be defined"
#endif

#ifndef SW_UART_WRAPPER_DT_RX_BUFFER_SIZE
#error "SW_UART_WRAPPER_DT_RX_BUFFER_SIZE should be defined"
#endif

#endif // CONFIG_SW_UART_WRAPPER_ENABLE_RX

extern uint8_t sw_uart_wrapper_ioctl(void * const aHandle ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t sw_uart_wrapper_callback(void * const aHandle ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2);
extern size_t sw_uart_wrapper_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);
#include "src/sw_uart_wrapper.h"

#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_DEV_NAME) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_CLIENT_PDEV) ;
	SW_UART_WRAPPER_Instance_t STATIC_DEVICE_INNER_INST(SW_UART_WRAPPER_DT_DEV_NAME) =	 {
				P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ,	/*server_dev*/
					NULL,			/* xQueue */
					1,				/* use_task_for_out */
				P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_DEV_NAME),			/* this_dev */
				P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_CLIENT_PDEV),	/* client_dev */
				SW_UART_WRAPPER_DT_RX_BUFFER,		/* rx_buff */
				SW_UART_WRAPPER_DT_RX_BUFFER_SIZE,	/* rx_buff_size */
					0,				/* WritePos */
					0,				/* ReadPos */
					0,				/*isDataInUse */
					0,				/*bufferWasOverflowed */
					0,				/* (*sendData) */
					0,				/*data_length*/
					NULL			/*xTX_WaitQueue*/
				};
#else // #ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ;
	SW_UART_WRAPPER_Instance_t STATIC_DEVICE_INNER_INST(SW_UART_WRAPPER_DT_DEV_NAME) =	 {
			P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ,	/*server_dev*/
				NULL,			/* xQueue */
				1,				/* use_task_for_out */
				0,				/* (*sendData) */
				0,				/*data_length*/
				NULL			/*xTX_WaitQueue*/
			};

#endif // for    #ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX

CREATE_STATIC_DEVICE(SW_UART_WRAPPER_DT_DEV_NAME ,
		&STATIC_DEVICE_INNER_INST(SW_UART_WRAPPER_DT_DEV_NAME)  ,
		sw_uart_wrapper_ioctl ,  sw_uart_wrapper_pwrite ,
		DEV_API_dummy_pread_func , sw_uart_wrapper_callback);




#undef CURRENT_DEV

#endif
