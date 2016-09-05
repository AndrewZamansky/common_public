/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "uart_stm32f10x_api.h"
#include  "src/uart_stm32f10x.h"

uint8_t uart_stm32f10x_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
size_t 	uart_stm32f10x_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME					uart_stm32f10x
#define	MODULE_IOCTL_FUNCTION		uart_stm32f10x_ioctl
#define	MODULE_PWRITE_FUNCTION		uart_stm32f10x_pwrite
#define MODULE_CONFIG_DATA_STRUCT_TYPE		UART_STM32F103x_Instance_t

#ifdef DT_DEV_MODULE

	#ifndef UART_STM32f10X_DT_UART_NUMBER
	#error "UART_STM32f10X_DT_UART_NUMBER should be defined"
	#endif

	#ifndef UART_STM32f10X_DT_CALLBACK_PDEV
	#error "UART_STM32f10X_DT_CALLBACK_PDEV should be defined"
	#endif

	#ifndef UART_STM32f10X_DT_BAUD_RATE
	#error "UART_STM32f10X_DT_BAUD_RATE should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(UART_STM32f10X_DT_CALLBACK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(UART_STM32f10X_DT_CALLBACK_PDEV) ,	\
			UART_STM32f10X_DT_UART_NUMBER ,								\
			UART_STM32f10X_DT_BAUD_RATE ,								\
			NULL														\
		}

#endif

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
			{IOCTL_UART_SET_BAUD_RATE , IOCTL_VOID , UART_API_BAUD_RATE_STR, NOT_FOR_SAVE},						\
			{IOCTL_UART_STM32F10X_SET_UART_NUM , IOCTL_VOID , UART_STM32F10X_API_UART_NUM_STR, NOT_FOR_SAVE},	\
			{IOCTL_UART_STM32F10X_DISABLE , IOCTL_VOID , UART_STM32F10X_API_UART_DISABLE_STR, NOT_FOR_SAVE}		\
	}


#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef UART_STM32f10X_DT_UART_NUMBER
#undef UART_STM32f10X_DT_CALLBACK_PDEV
#undef UART_STM32f10X_DT_BAUD_RATE

