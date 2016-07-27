/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "sw_uart_wrapper_api.h"
#include "src/sw_uart_wrapper.h"

extern uint8_t sw_uart_wrapper_ioctl(void * const aHandle ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t sw_uart_wrapper_callback(void * const aHandle ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2);
extern size_t sw_uart_wrapper_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME					sw_uart_wrapper
#define	MODULE_IOCTL_FUNCTION		sw_uart_wrapper_ioctl
#define	MODULE_PWRITE_FUNCTION		sw_uart_wrapper_pwrite
#define	MODULE_CALLBACK_FUNCTION	sw_uart_wrapper_callback
#define MODULE_DATA_STRUCT_TYPE		SW_UART_WRAPPER_Instance_t

#ifdef DT_DEV_MODULE

	#ifndef SW_UART_WRAPPER_DT_SERVER_PDEV
		#error "SW_UART_WRAPPER_DT_SERVER_PDEV should be defined"
	#endif

	#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX

	#ifndef SW_UART_WRAPPER_DT_CLIENT_PDEV
		#error "SW_UART_WRAPPER_DT_CLIENT_PDEV should be defined"
	#endif

	#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
		#ifndef SW_UART_WRAPPER_DT_RX_BUFFER_SIZE
			#error "SW_UART_WRAPPER_DT_RX_BUFFER_SIZE should be defined"
		#endif
	#endif

	#endif // CONFIG_SW_UART_WRAPPER_ENABLE_RX


	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_CLIENT_PDEV) ;

	#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
		#define RX_BUFFER_STRUCT_DATA									\
				SW_UART_WRAPPER_DT_RX_BUFFER_SIZE ,	/*.rx_buff_size*/	\
				NULL,								/* .rx_buff */
	#else
		#define RX_BUFFER_STRUCT_DATA									\
				{0} ,								/*.rx_buff*/
	#endif

	#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
		#define RX_STRUCT_DATA						\
				P_TO_STATIC_DEVICE_INST(DT_DEV_NAME),			/* .this_dev */	\
				P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_CLIENT_PDEV),	/* .client_dev */	\
				RX_BUFFER_STRUCT_DATA							\
					0,				/* .WritePos */				\
					0,				/* .ReadPos */				\
					0,				/* .isDataInUse */			\
					0,				/* .bufferWasOverflowed */
	#else
		#define RX_STRUCT_DATA
	#endif

	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ,	/*server_dev*/ \
				NULL,			/* xQueue */				\
				1,				/* use_task_for_out */		\
				RX_STRUCT_DATA								\
				0,				/* (*sendData) */			\
				0,				/*data_length*/				\
				NULL			/*xTX_WaitQueue*/			\
		}

#endif


#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
				{IOCTL_SW_UART_WRAPPER_SET_BUFF_SIZE, IOCTL_VOID , SW_UART_WRAPPER_API_RX_BUFF_SIZE_STR , NOT_FOR_SAVE},\
				{IOCTL_SET_SERVER_DEVICE_BY_NAME 	, IOCTL_VOID , SW_UART_WRAPPER_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},\
		}

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef SW_UART_WRAPPER_DT_SERVER_PDEV
#undef SW_UART_WRAPPER_DT_CLIENT_PDEV
#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
	#undef SW_UART_WRAPPER_DT_RX_BUFFER_SIZE
#endif
