#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "uart_nuc505_api.h"

#undef _add_static_device_step2

#undef UART_NUC505_DT_DEV_NAME
#undef UART_NUC505_DT_UART_NUMBER
#undef UART_NUC505_DT_CALLBACK_PDEV
#undef UART_NUC505_DT_BAUD_RATE


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef UART_NUC505_DT_DEV_NAME
#error "UART_NUC505_DT_DEV_NAME should be defined"
#endif

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
UART_NUC505_Instance_t STATIC_DEVICE_INNER_INST(UART_NUC505_DT_DEV_NAME) =
	{
			P_TO_STATIC_DEVICE_INST(UART_NUC505_DT_CALLBACK_PDEV) ,
			UART_NUC505_DT_UART_NUMBER ,
			UART_NUC505_DT_BAUD_RATE
	};
CREATE_STATIC_DEVICE(UART_NUC505_DT_DEV_NAME , &STATIC_DEVICE_INNER_INST(UART_NUC505_DT_DEV_NAME) ,
		uart_nuc505_ioctl , uart_nuc505_pwrite ,
		DEV_API_dummy_pread_func , DEV_API_dummy_callback_func);



#undef CURRENT_DEV

#endif
