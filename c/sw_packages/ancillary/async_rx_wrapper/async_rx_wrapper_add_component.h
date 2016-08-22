/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "async_rx_wrapper_api.h"
#include "src/async_rx_wrapper.h"

extern uint8_t async_rx_wrapper_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t async_rx_wrapper_callback(pdev_descriptor_t apdev ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2);

#define	MODULE_NAME							async_rx_wrapper
#define	MODULE_IOCTL_FUNCTION				async_rx_wrapper_ioctl
#define	MODULE_CALLBACK_FUNCTION			async_rx_wrapper_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE		async_rx_wrapper_instance_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE		async_rx_wrapper_runtime_instance_t

#ifdef DT_DEV_MODULE

	#ifndef ASYNC_RX_WRAPPER_DT_SERVER_PDEV
		#error "ASYNC_RX_WRAPPER_DT_SERVER_PDEV should be defined"
	#endif


	#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
		#ifndef ASYNC_RX_WRAPPER_DT_RX_BUFFER_SIZE
			#error "ASYNC_RX_WRAPPER_DT_RX_BUFFER_SIZE should be defined"
		#endif
	#endif



#ifdef ASYNC_RX_WRAPPER_DT_CLIENT_PDEV
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(ASYNC_RX_WRAPPER_DT_CLIENT_PDEV) ;
	#define POINTER_TO_CLIENT_PDEV		P_TO_STATIC_DEVICE_INST(ASYNC_RX_WRAPPER_DT_CLIENT_PDEV)
#else
	#define POINTER_TO_CLIENT_PDEV		NULL
#endif
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(ASYNC_RX_WRAPPER_DT_SERVER_PDEV) ;

	#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
		#define RX_BUFFER_STRUCT_DATA									\
				ASYNC_RX_WRAPPER_DT_RX_BUFFER_SIZE ,	/*.rx_buff_size*/	\
				NULL								/* .rx_buff */
	#else
		#define RX_BUFFER_STRUCT_DATA									\
				{0}								/*.rx_buff*/
	#endif

	#define RX_STRUCT_DATA									\
			POINTER_TO_CLIENT_PDEV,	/* .client_dev */		\
			RX_BUFFER_STRUCT_DATA

	#define STATIC_DEV_DATA_STRUCT														\
		{																				\
			P_TO_STATIC_DEVICE_INST(ASYNC_RX_WRAPPER_DT_SERVER_PDEV) ,	/*server_dev*/	\
			RX_STRUCT_DATA																\
		}

#endif


#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
				{IOCTL_ASYNC_RX_WRAPPER_SET_BUFF_SIZE, IOCTL_VOID , ASYNC_RX_WRAPPER_API_RX_BUFF_SIZE_STR , NOT_FOR_SAVE},\
				{IOCTL_SET_SERVER_DEVICE_BY_NAME 	, IOCTL_VOID , ASYNC_RX_WRAPPER_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},\
		}

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef ASYNC_RX_WRAPPER_DT_SERVER_PDEV
#undef ASYNC_RX_WRAPPER_DT_CLIENT_PDEV
#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
	#undef ASYNC_RX_WRAPPER_DT_RX_BUFFER_SIZE
#endif