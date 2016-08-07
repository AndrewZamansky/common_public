/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "sw_uart_wrapper_api.h"
#include "src/sw_uart_wrapper.h"

extern uint8_t sw_uart_wrapper_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t sw_uart_wrapper_callback(pdev_descriptor_t apdev ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2);
extern size_t sw_uart_wrapper_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME							sw_uart_wrapper
#define	MODULE_IOCTL_FUNCTION				sw_uart_wrapper_ioctl
#define	MODULE_PWRITE_FUNCTION				sw_uart_wrapper_pwrite
#define	MODULE_CALLBACK_FUNCTION			sw_uart_wrapper_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE		sw_uart_wrapper_instance_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE		sw_uart_wrapper_runtime_instance_t

#ifdef DT_DEV_MODULE

	#ifndef SW_UART_WRAPPER_DT_SERVER_PDEV
		#error "SW_UART_WRAPPER_DT_SERVER_PDEV should be defined"
	#endif

	#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX

	#ifndef SW_UART_WRAPPER_USE_TASK_FOR_TX
		#define SW_UART_WRAPPER_USE_TASK_FOR_TX		1
	#endif

	#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
		#ifndef SW_UART_WRAPPER_DT_RX_BUFFER_SIZE
			#error "SW_UART_WRAPPER_DT_RX_BUFFER_SIZE should be defined"
		#endif
	#endif

	#endif // CONFIG_SW_UART_WRAPPER_ENABLE_RX


#ifdef SW_UART_WRAPPER_DT_CLIENT_PDEV
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_CLIENT_PDEV) ;
	#define POINTER_TO_CLIENT_PDEV		P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_CLIENT_PDEV)
#else
	#define POINTER_TO_CLIENT_PDEV		NULL
#endif
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ;

	#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
		#define RX_BUFFER_STRUCT_DATA									\
				SW_UART_WRAPPER_DT_RX_BUFFER_SIZE ,	/*.rx_buff_size*/	\
				NULL								/* .rx_buff */
	#else
		#define RX_BUFFER_STRUCT_DATA									\
				{0}								/*.rx_buff*/
	#endif

	#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
		#define RX_STRUCT_DATA									\
				POINTER_TO_CLIENT_PDEV,	/* .client_dev */		\
				RX_BUFFER_STRUCT_DATA
	#else
		#define RX_STRUCT_DATA
	#endif

	#define STATIC_DEV_DATA_STRUCT														\
		{																				\
			P_TO_STATIC_DEVICE_INST(SW_UART_WRAPPER_DT_SERVER_PDEV) ,	/*server_dev*/	\
			RX_STRUCT_DATA	,															\
			SW_UART_WRAPPER_USE_TASK_FOR_TX						/*use_task_for_out	*/	\
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
