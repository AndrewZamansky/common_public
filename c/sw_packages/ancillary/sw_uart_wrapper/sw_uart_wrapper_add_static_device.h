/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "sw_uart_wrapper_api.h"


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





EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ;
#define STATIC_DEV_DATA_STRUCT_TYPE	SW_UART_WRAPPER_Instance_t
#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(DT_DEV_NAME) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_CLIENT_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ,	/*server_dev*/	\
				NULL,			/* xQueue */						\
				1,				/* use_task_for_out */				\
			P_TO_STATIC_DEVICE_INST(DT_DEV_NAME),			/* this_dev */	\
			P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_CLIENT_PDEV),	/* client_dev */	\
			SW_UART_WRAPPER_DT_RX_BUFFER,		/* rx_buff */								\
			SW_UART_WRAPPER_DT_RX_BUFFER_SIZE,	/* rx_buff_size */							\
				0,				/* WritePos */				\
				0,				/* ReadPos */				\
				0,				/*isDataInUse */			\
				0,				/*bufferWasOverflowed */	\
				0,				/* (*sendData) */			\
				0,				/*data_length*/				\
				NULL			/*xTX_WaitQueue*/			\
		}
#else // #ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX

	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ,	/*server_dev*/ \
				NULL,			/* xQueue */				\
				1,				/* use_task_for_out */		\
				0,				/* (*sendData) */			\
				0,				/*data_length*/				\
				NULL			/*xTX_WaitQueue*/			\
		}

#endif // for    #ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
#define	STATIC_DEV_IOCTL_FUNCTION		sw_uart_wrapper_ioctl
#define	STATIC_DEV_PWRITE_FUNCTION		sw_uart_wrapper_pwrite
#define	STATIC_DEV_CALLBACK_FUNCTION	sw_uart_wrapper_callback
#include "add_static_dev.h"


#undef SW_UART_WRAPPER_DT_SERVER_PDEV
#undef SW_UART_WRAPPER_DT_CLIENT_PDEV
#undef SW_UART_WRAPPER_DT_RX_BUFFER
#undef SW_UART_WRAPPER_DT_RX_BUFFER_SIZE

