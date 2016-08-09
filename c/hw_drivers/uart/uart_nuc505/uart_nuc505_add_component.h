/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "uart_nuc505_api.h"
#include 	"src/uart_nuc505.h"

uint8_t uart_nuc505_callback(pdev_descriptor_t apdev , const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);
uint8_t uart_nuc505_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t uart_nuc505_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME					uart_nuc505
#define	MODULE_IOCTL_FUNCTION		uart_nuc505_ioctl
#define	MODULE_CALLBACK_FUNCTION	uart_nuc505_callback
#define	MODULE_PWRITE_FUNCTION		uart_nuc505_pwrite
#define MODULE_CONFIG_DATA_STRUCT_TYPE		uart_nuc505_instance_t

#ifdef DT_DEV_MODULE

	#ifndef UART_NUC505_DT_BASE_ADDRESS
		#error "UART_NUC505_DT_BASE_ADDRESS should be defined"
	#endif

	#ifndef UART_NUC505_DT_CALLBACK_PDEV
		#error "UART_NUC505_DT_CALLBACK_PDEV should be defined"
	#endif

	#ifndef UART_NUC505_DT_BAUD_RATE
		#error "UART_NUC505_DT_BAUD_RATE should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(UART_NUC505_DT_CALLBACK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(UART_NUC505_DT_CALLBACK_PDEV) ,	\
			UART_NUC505_DT_BASE_ADDRESS ,							\
			UART_NUC505_DT_BAUD_RATE 								\
		}

	#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef UART_NUC505_DT_UART_NUMBER
#undef UART_NUC505_DT_CALLBACK_PDEV
#undef UART_NUC505_DT_BAUD_RATE

