/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "ipc_i96xxx_api.h"
#include  "src/ipc_i96xxx.h"

#ifndef IPC_I96XXX_DT_TIMER_PDEV
	#error "IPC_I96XXX_DT_TIMER_PDEV should be defined"
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(IPC_I96XXX_DT_TIMER_PDEV);
#define POINTER_TO_TIMER_PDEV  P_TO_STATIC_DEVICE_INST(IPC_I96XXX_DT_TIMER_PDEV)


SET_STATIC_DEV_CONFIG(ipc_i96xxx) =
{
	POINTER_TO_TIMER_PDEV,
};

#include "add_static_dev.h"

/* device specific defines should be undefined
 * after calling #include "add_static_dev.h"
 */
#undef POINTER_TO_UART_RX_PDEV
#undef POINTER_TO_UART_TX_PDEV
#undef POINTER_TO_TIMER_PDEV
#undef IPC_I96XXX_DT_TIMER_PDEV
#undef IPC_I96XXX_DT_UART_TX_DEV
#undef IPC_I96XXX_DT_UART_RX_DEV
#undef IPC_I96XXX_DT_UART_DEV
#undef POINTER_TO_UART_PDEV
