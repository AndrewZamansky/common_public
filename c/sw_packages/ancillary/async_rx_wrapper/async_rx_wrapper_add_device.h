/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "async_rx_wrapper_api.h"
#include "src/async_rx_wrapper.h"

#define MODULE_CONFIG_DATA_STRUCT_TYPE    struct async_rx_wrapper_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE   struct async_rx_wrapper_runtime_t


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
	#define POINTER_TO_CLIENT_PDEV  \
					P_TO_STATIC_DEVICE_INST(ASYNC_RX_WRAPPER_DT_CLIENT_PDEV)
#else
	#define POINTER_TO_CLIENT_PDEV  NULL
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(ASYNC_RX_WRAPPER_DT_SERVER_PDEV) ;

#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
	#define RX_BUFFER_STRUCT_DATA   NULL    /* .*rx_buff */
#else
	#define RX_BUFFER_STRUCT_DATA   {0}     /*.rx_buff[]*/
#endif


#define STATIC_DEV_DATA_STRUCT                                    \
	{                                                             \
		P_TO_STATIC_DEVICE_INST(ASYNC_RX_WRAPPER_DT_SERVER_PDEV) ,\
		POINTER_TO_CLIENT_PDEV,            /* .client_dev */      \
		ASYNC_RX_WRAPPER_DT_RX_BUFFER_SIZE,                       \
		RX_BUFFER_STRUCT_DATA                                     \
	}




#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef POINTER_TO_CLIENT_PDEV
#undef RX_BUFFER_STRUCT_DATA
#undef ASYNC_RX_WRAPPER_DT_SERVER_PDEV
#undef ASYNC_RX_WRAPPER_DT_CLIENT_PDEV
#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
	#undef ASYNC_RX_WRAPPER_DT_RX_BUFFER_SIZE
#endif
