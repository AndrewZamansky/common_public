/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "uart_i94xxx_api.h"
#include 	"src/uart_i94xxx.h"


#define	MODULE_NAME							uart_i94xxx
#define	MODULE_IOCTL_FUNCTION				uart_i94xxx_ioctl
#define	MODULE_CALLBACK_FUNCTION			uart_i94xxx_callback
#define	MODULE_PWRITE_FUNCTION				uart_i94xxx_pwrite
#define MODULE_CONFIG_DATA_STRUCT_TYPE		struct uart_i94xxx_cfg_t

#ifdef DT_DEV_MODULE

	#ifndef UART_I94XXX_DT_BASE_ADDRESS
		#error "UART_I94XXX_DT_BASE_ADDRESS should be defined"
	#endif

	#ifdef UART_I94XXX_DT_TX_CALLBACK_PDEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
							UART_I94XXX_DT_TX_CALLBACK_PDEV) ;
		#define POINTER_TO_TX_CALLBACK_PDEV		\
				P_TO_STATIC_DEVICE_INST(UART_I94XXX_DT_TX_CALLBACK_PDEV)
	#else
		#warning "UART_I94XXX_DT_TX_CALLBACK_PDEV not defined in device tree"
		#define POINTER_TO_TX_CALLBACK_PDEV		NULL
	#endif

	#ifdef UART_I94XXX_DT_RX_CALLBACK_PDEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(
							UART_I94XXX_DT_RX_CALLBACK_PDEV) ;
		#define POINTER_TO_RX_CALLBACK_PDEV		\
		P_TO_STATIC_DEVICE_INST(UART_I94XXX_DT_RX_CALLBACK_PDEV)
	#else
		#warning "UART_I94XXX_DT_RX_CALLBACK_PDEV not defined in device tree"
		#define POINTER_TO_RX_CALLBACK_PDEV		NULL
	#endif

	#ifndef UART_I94XXX_DT_BAUD_RATE
		#error "UART_I94XXX_DT_BAUD_RATE should be defined"
	#endif

	#ifndef UART_I94XXX_DT_SRC_CLOCK_PDEV
		#error "UART_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
	#endif
	#define POINTER_TO_SRC_CLOCK_PDEV	 \
			P_TO_STATIC_DEVICE_INST(UART_I94XXX_DT_SRC_CLOCK_PDEV)

	#define STATIC_DEV_DATA_STRUCT			\
		{									\
			POINTER_TO_TX_CALLBACK_PDEV ,	\
			POINTER_TO_RX_CALLBACK_PDEV ,	\
			POINTER_TO_SRC_CLOCK_PDEV 	,	\
			UART_I94XXX_DT_BASE_ADDRESS ,	\
			UART_I94XXX_DT_BAUD_RATE 		\
		}

	#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef POINTER_TO_SRC_CLOCK_PDEV
#undef POINTER_TO_TX_CALLBACK_PDEV
#undef POINTER_TO_RX_CALLBACK_PDEV
#undef UART_I94XXX_DT_UART_NUMBER
#undef UART_I94XXX_DT_CALLBACK_PDEV
#undef UART_I94XXX_DT_BAUD_RATE
#undef UART_I94XXX_DT_SRC_CLOCK_PDEV

