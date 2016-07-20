/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "uart_nuc505_api.h"

#ifndef UART_NUC505_DT_UART_NUMBER
#error "UART_NUC505_DT_UART_NUMBER should be defined"
#endif

#ifndef UART_NUC505_DT_CALLBACK_PDEV
#error "UART_NUC505_DT_CALLBACK_PDEV should be defined"
#endif

#ifndef UART_NUC505_DT_BAUD_RATE
#error "UART_NUC505_DT_BAUD_RATE should be defined"
#endif

#include "src/uart_nuc505.h"
uint8_t uart_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t uart_nuc505_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(UART_NUC505_DT_CALLBACK_PDEV) ;

#define STATIC_DEV_DATA_STRUCT_TYPE		UART_NUC505_Instance_t
#define STATIC_DEV_DATA_STRUCT									\
	{															\
		P_TO_STATIC_DEVICE_INST(UART_NUC505_DT_CALLBACK_PDEV) ,	\
		UART_NUC505_DT_UART_NUMBER ,							\
		UART_NUC505_DT_BAUD_RATE 								\
	}

#define	STATIC_DEV_IOCTL_FUNCTION		uart_nuc505_ioctl
#define	STATIC_DEV_PWRITE_FUNCTION		uart_nuc505_pwrite
#include "add_static_dev.h"

#undef UART_NUC505_DT_UART_NUMBER
#undef UART_NUC505_DT_CALLBACK_PDEV
#undef UART_NUC505_DT_BAUD_RATE

