/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "async_tx_wrapper_api.h"
#include "src/async_tx_wrapper.h"

extern uint8_t async_tx_wrapper_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
extern uint8_t async_tx_wrapper_callback(pdev_descriptor_t apdev ,const uint8_t aCallback_num ,
		void * aCallback_param1, void * aCallback_param2);
extern size_t async_tx_wrapper_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME							async_tx_wrapper
#define	MODULE_IOCTL_FUNCTION				async_tx_wrapper_ioctl
#define	MODULE_PWRITE_FUNCTION				async_tx_wrapper_pwrite
#define	MODULE_CALLBACK_FUNCTION			async_tx_wrapper_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE		async_tx_wrapper_instance_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE		async_tx_wrapper_runtime_instance_t

#ifdef DT_DEV_MODULE

	#ifndef ASYNC_TX_WRAPPER_DT_SERVER_PDEV
		#error "ASYNC_TX_WRAPPER_DT_SERVER_PDEV should be defined"
	#endif


	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(ASYNC_TX_WRAPPER_DT_SERVER_PDEV) ;

	#define STATIC_DEV_DATA_STRUCT														\
		{																				\
			P_TO_STATIC_DEVICE_INST(ASYNC_TX_WRAPPER_DT_SERVER_PDEV) 	/*server_dev*/	\
		}

#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
			{"async_tx_server" 	, IOCTL_SET_SERVER_DEVICE 	, IOCTL_VOID , \
				DEV_PARAM_TYPE_PDEVICE , MAPPED_SET_DUMMY_PARAM() },\
		}


#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef ASYNC_TX_WRAPPER_DT_SERVER_PDEV
