/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "uart_stm8_api.h"
#include "src/uart_stm8.h"

uint8_t uart_stm8_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t uart_stm8_pwrite( struct dev_desc_t *adev ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME					uart_stm8
#define	MODULE_IOCTL_FUNCTION		uart_stm8_ioctl
#define	MODULE_PWRITE_FUNCTION		uart_stm8_pwrite
#define MODULE_CONFIG_DATA_STRUCT_TYPE		uart_stm8_instance_t

#ifdef DT_DEV_MODULE

	#ifndef UART_STM8_DT_TX_CALLBACK_PDEV
		#warning "UART_STM8_DT_TX_CALLBACK_PDEV not defined"
		#define		TX_CALLBACK_PDEV	NULL
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(UART_STM8_DT_TX_CALLBACK_PDEV) ;
		#define		TX_CALLBACK_PDEV	P_TO_STATIC_DEVICE_INST(UART_STM8_DT_TX_CALLBACK_PDEV)
	#endif

	#ifndef UART_STM8_DT_RX_CALLBACK_PDEV
		//#warning "UART_STM8_DT_RX_CALLBACK_PDEV not defined"
		#define		RX_CALLBACK_PDEV	NULL
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(UART_STM8_DT_RX_CALLBACK_PDEV) ;
		#define		RX_CALLBACK_PDEV	P_TO_STATIC_DEVICE_INST(UART_STM8_DT_RX_CALLBACK_PDEV)
	#endif

	#ifndef UART_STM8_DT_BAUD_RATE
		#error "UART_STM8_DT_BAUD_RATE should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT		\
		{								\
			TX_CALLBACK_PDEV,			\
			RX_CALLBACK_PDEV,			\
			UART_STM8_DT_BAUD_RATE		\
		}

	#endif

#include "add_static_dev.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef RX_CALLBACK_PDEV
#undef TX_CALLBACK_PDEV
#undef UART_STM8_DT_CALLBACK_PDEV
#undef UART_STM8_DT_BAUD_RATE

