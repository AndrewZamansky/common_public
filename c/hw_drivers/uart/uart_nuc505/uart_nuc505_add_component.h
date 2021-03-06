/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "uart_nuc505_api.h"
#include  "src/uart_nuc505.h"


#define	MODULE_NAME                      uart_nuc505
#define	MODULE_IOCTL_FUNCTION            uart_nuc505_ioctl
#define	MODULE_CALLBACK_FUNCTION         uart_nuc505_callback
#define	MODULE_PWRITE_FUNCTION           uart_nuc505_pwrite
#define MODULE_CONFIG_DATA_STRUCT_TYPE   struct uart_nuc505_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE  struct uart_nuc505_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef UART_NUC505_DT_BASE_ADDRESS
		#error "UART_NUC505_DT_BASE_ADDRESS should be defined"
	#endif

	#ifdef UART_NUC505_DT_TX_CALLBACK_PDEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(UART_NUC505_DT_TX_CALLBACK_PDEV) ;
		#define POINTER_TO_TX_CALLBACK_PDEV   P_TO_STATIC_DEVICE_INST(UART_NUC505_DT_TX_CALLBACK_PDEV)
	#else
		#pragma message "UART_NUC505_DT_TX_CALLBACK_PDEV not defined in device tree"
		#define POINTER_TO_TX_CALLBACK_PDEV   NULL
	#endif

	#ifdef UART_NUC505_DT_RX_CALLBACK_PDEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(UART_NUC505_DT_RX_CALLBACK_PDEV) ;
		#define POINTER_TO_RX_CALLBACK_PDEV    P_TO_STATIC_DEVICE_INST(UART_NUC505_DT_RX_CALLBACK_PDEV)
	#else
		#pragma message "UART_NUC505_DT_RX_CALLBACK_PDEV not defined in device tree"
		#define POINTER_TO_RX_CALLBACK_PDEV    NULL
	#endif

	#ifndef UART_NUC505_DT_BAUD_RATE
		#error "UART_NUC505_DT_BAUD_RATE should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT          \
		{                                   \
			POINTER_TO_TX_CALLBACK_PDEV ,   \
			POINTER_TO_RX_CALLBACK_PDEV ,   \
			UART_NUC505_DT_BASE_ADDRESS ,   \
			UART_NUC505_DT_BAUD_RATE        \
		}

	#endif

#include "add_static_dev.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef POINTER_TO_TX_CALLBACK_PDEV
#undef POINTER_TO_RX_CALLBACK_PDEV
#undef UART_NUC505_DT_UART_NUMBER
#undef UART_NUC505_DT_BAUD_RATE
#undef UART_NUC505_DT_BASE_ADDRESS
#undef UART_NUC505_DT_TX_CALLBACK_PDEV
#undef UART_NUC505_DT_RX_CALLBACK_PDEV
