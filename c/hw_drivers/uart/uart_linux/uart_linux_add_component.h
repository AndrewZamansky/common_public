/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "uart_linux_api.h"
#include     "src/uart_linux.h"


#define  MODULE_NAME                  uart_linux
#define  MODULE_IOCTL_FUNCTION        uart_linux_ioctl
#define  MODULE_PWRITE_FUNCTION       uart_linux_pwrite
#define MODULE_RUNTIME_DATA_STRUCT_TYPE  struct uart_linux_runtime_t
#define  MODULE_CONFIG_DATA_STRUCT_TYPE  struct uart_linux_cfg_t

#ifdef DT_DEV_MODULE


	#ifdef UART_LINUX_DT_TX_CALLBACK_PDEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
							UART_LINUX_DT_TX_CALLBACK_PDEV) ;
		#define POINTER_TO_TX_CALLBACK_PDEV        \
				P_TO_STATIC_DEVICE_INST(UART_LINUX_DT_TX_CALLBACK_PDEV)
	#else
		#warning "UART_LINUX_DT_TX_CALLBACK_PDEV not defined in device tree"
		#define POINTER_TO_TX_CALLBACK_PDEV        NULL
	#endif

	#ifdef UART_LINUX_DT_RX_CALLBACK_PDEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
							UART_LINUX_DT_RX_CALLBACK_PDEV) ;
		#define POINTER_TO_RX_CALLBACK_PDEV        \
		P_TO_STATIC_DEVICE_INST(UART_LINUX_DT_RX_CALLBACK_PDEV)
	#else
		#warning "UART_LINUX_DT_RX_CALLBACK_PDEV not defined in device tree"
		#define POINTER_TO_RX_CALLBACK_PDEV        NULL
	#endif

	#ifndef UART_LINUX_DT_BAUD_RATE
		#error "UART_LINUX_DT_BAUD_RATE should be defined"
	#endif

	#ifndef UART_LINUX_DT_TTY_DEV_PATH
		#error "UART_LINUX_DT_TTY_DEV_PATH should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT          \
		{                                   \
			POINTER_TO_TX_CALLBACK_PDEV ,   \
			POINTER_TO_RX_CALLBACK_PDEV ,   \
			UART_LINUX_DT_BAUD_RATE,        \
			UART_LINUX_DT_TTY_DEV_PATH,     \
		}

	#endif

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef POINTER_TO_TX_CALLBACK_PDEV
#undef POINTER_TO_RX_CALLBACK_PDEV
#undef UART_LINUX_DT_TX_CALLBACK_PDEV
#undef UART_LINUX_DT_RX_CALLBACK_PDEV
#undef UART_LINUX_DT_BAUD_RATE
#undef UART_LINUX_DT_TTY_DEV_PATH
