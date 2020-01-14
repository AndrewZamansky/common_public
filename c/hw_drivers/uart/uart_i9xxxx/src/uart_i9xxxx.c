/*
 *
 * uart_i9xxxx.c

 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#ifdef CONFIG_I94XXX
	#include "I94100.h"
	#include "clock_control_i94xxx_api.h"
#elif CONFIG_I96XXX_M0
	#include "I96100.h"
	#include "clock_control_i96xxx_m0_api.h"
#endif

#include "irq_api.h"
#include "uart_api.h"
#include "pin_control_api.h"
#include <stdio.h>

#include "uart_i9xxxx_api.h"

//#define DEBUG
#include "PRINTF_api.h"
#include "uart_i9xxxx.h"


#if !defined(INTERRUPT_PRIORITY_FOR_UART)
	#error "INTERRUPT_PRIORITY_FOR_UART should be defined"
#endif

#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_UART)
	#error "priority should be lower then maximal priority for os syscalls"
#endif



//#define MAX_ERR_STR_LEN   255

#define DEBUG_UART  0
//#define PRINT_BIN_DATA


uint8_t _do_uart_dbg_print = 0;
#if (0 != DEBUG_UART)
static void dbg_print(uint8_t const *data, size_t len)
{
	if (0 == _do_uart_dbg_print) return;

	#ifdef PRINT_BIN_DATA
		size_t i;
		for (i = 0; i < len; i++)
		{
			char dbg_str[] = "0x00 ";
			snprintf(dbg_str, sizeof(dbg_str), "0x%02x ", *data);
			PRINT_DATA_DBG_ISR(dbg_str, sizeof(dbg_str) - 1);
			data++;
		}
	#else
		PRINT_DATA_DBG_ISR( data, len);
	#endif
}
#else
static void dbg_print(uint8_t const *data, size_t len){}
#endif


static uint8_t uart_i9xxxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct uart_i9xxxx_cfg_t *cfg_hndl;
	struct i9xxx_uart_runtime_t *i9xxx_uart_runtime;
	uint32_t u32IntSts ;
	UART_T *uart_regs;
	size_t rcv_data_len;
	struct dev_desc_t * callback_rx_dev ;
	uint8_t  *rcv_data;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(uart_i9xxxx, adev);
	i9xxx_uart_runtime = DEV_GET_RUNTIME_DATA_POINTER(uart_i9xxxx, adev);

	uart_regs = (UART_T *)cfg_hndl->base_address;
	rcv_data = i9xxx_uart_runtime->rcv_data;
	u32IntSts = uart_regs->INTSTS;

	rcv_data_len = 0;

	callback_rx_dev = cfg_hndl->callback_rx_dev;

	if (uart_regs->FIFOSTS & UART_FIFOSTS_RXOVIF_Msk)
	{
		//CRITICAL_ERROR("uart rx FIFO was overflowed");
		i9xxx_uart_runtime->rx_fifo_overflow_cnt++;
		uart_regs->FIFOSTS = UART_FIFOSTS_RXOVIF_Msk;
	}
	while (0 == UART_GET_RX_EMPTY(uart_regs))
	{
		rcv_data[rcv_data_len] = UART_READ(uart_regs);
		rcv_data_len++;
		if ( (UART_I9XXXX_RCV_DATA_SIZE_BUFFER == rcv_data_len) &&
				(NULL != callback_rx_dev))
		{
			if ((DEBUG_UART) && (0 != _do_uart_dbg_print))
			{
				dbg_print(rcv_data, rcv_data_len);
				if (0 != _do_uart_dbg_print)
				{
					PRINT_DATA_DBG_ISR("|e.rcv0|\n", sizeof("|e.rcv0|\n") - 1);
				}
			}
			DEV_CALLBACK_2_PARAMS(callback_rx_dev,
				CALLBACK_DATA_RECEIVED,  rcv_data, (void*)(size_t)rcv_data_len);
			rcv_data_len = 0;// start getting data again
		}
	}


	if ( rcv_data_len && (NULL != callback_rx_dev))
	{
		if ((DEBUG_UART) && (0 != _do_uart_dbg_print))
		{
			PRINT_DATA_DBG_ISR("{", 1);
			dbg_print(rcv_data, rcv_data_len);
			PRINT_DATA_DBG_ISR("}", 1);
		}
		DEV_CALLBACK_2_PARAMS(callback_rx_dev,
				CALLBACK_DATA_RECEIVED,  rcv_data,  (void*)rcv_data_len);
	}


	if ( (uart_regs->INTEN & UART_INTEN_TXENDIEN_Msk) &&
						(u32IntSts & UART_INTEN_TXENDIEN_Msk))
	{
		struct dev_desc_t * callback_tx_dev ;

		callback_tx_dev = cfg_hndl->callback_tx_dev;
		UART_DISABLE_INT(uart_regs,  UART_INTEN_TXENDIEN_Msk);
		if (NULL != callback_tx_dev)
		{
			DEV_CALLBACK_1_PARAMS( callback_tx_dev,
					CALLBACK_TX_DONE, (void*)i9xxx_uart_runtime->send_data_len);
		}
	}

	return 0;
}


/**
 * uart_i9xxxx_pwrite()
 *
 * return:
 */
static size_t uart_i9xxxx_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct uart_i9xxxx_cfg_t *cfg_hndl;
	struct i9xxx_uart_runtime_t *i9xxx_uart_runtime;
	size_t   send_data_len;

	UART_T *uart_regs;
#if (0 != DEBUG_UART)
	const uint8_t *start_of_data = apData;
#endif

	if (0 == aLength)
	{
		return 0;
	}

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(uart_i9xxxx, adev);
	i9xxx_uart_runtime = DEV_GET_RUNTIME_DATA_POINTER(uart_i9xxxx, adev);
	uart_regs = (UART_T *)cfg_hndl->base_address;

	// wait till busy
	while ((uart_regs->INTEN & UART_INTEN_TXENDIEN_Msk) ||
			((uart_regs->FIFOSTS & UART_FIFOSTS_TXEMPTYF_Msk) == 0));

	send_data_len = 0;
	while ( aLength && (0 == UART_IS_TX_FULL(uart_regs)))
	{
		send_data_len++;
		aLength--;
		UART_WRITE(uart_regs, *apData++);
	}
	i9xxx_uart_runtime->send_data_len = send_data_len;
	UART_EnableInt(uart_regs,  UART_INTEN_TXENDIEN_Msk );

	if ((DEBUG_UART) && (0 != _do_uart_dbg_print))
	{
		char uart_send_end_dbg_str[] = "uart send_data_len = 00000\n";
		snprintf(uart_send_end_dbg_str, sizeof(uart_send_end_dbg_str),
				"uart send_data_len = %05lu\n", (uint32_t)send_data_len);
//		PRINT_DATA_DBG_ISR(start_of_data, send_data_len);
//		PRINT_DATA_DBG_ISR(uart_send_end_dbg_str,
//										sizeof(uart_send_end_dbg_str)- 1);
	}

	return send_data_len;

}

/**
 * uart_i9xxxx_ioctl()
 *
 * return:
 */
static uint8_t uart_i9xxxx_ioctl( struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct uart_i9xxxx_cfg_t *cfg_hndl;
	struct i9xxx_uart_runtime_t *i9xxx_uart_runtime;
	UART_T *uart_regs;
	int uart_irq;
	uint32_t uart_module_rst;
	struct dev_desc_t  *src_clock;
	struct dev_desc_t  *uart_clock;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(uart_i9xxxx, adev);
	i9xxx_uart_runtime = DEV_GET_RUNTIME_DATA_POINTER(uart_i9xxxx, adev);
	if ((0 == i9xxx_uart_runtime->init_done) &&
					(IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	uart_regs = (UART_T *)cfg_hndl->base_address;
	switch(aIoctl_num)
	{
	case IOCTL_UART_SET_BAUD_RATE :
		cfg_hndl->baud_rate = *(uint32_t*)aIoctl_param1;
		UART_Open(uart_regs, cfg_hndl->baud_rate);
		break;
	case IOCTL_DEVICE_START :
		if (i9xxx_uart_runtime->init_done)
		{
			break;
		}
		pin_control_api_set_pin_function(cfg_hndl->tx_pin);
		pin_control_api_set_pin_function(cfg_hndl->rx_pin);

		src_clock = cfg_hndl->src_clock;
		if ((UART_T *)UART0_BASE == uart_regs)
		{
			uart_clock = i9xxxx_uart0_clk_dev;
			uart_module_rst = UART0_RST;
			uart_irq = UART0_IRQn;
		}
		else
		{
			CRITICAL_ERROR("TODO");
		}
		DEV_IOCTL_0_PARAMS(uart_clock, IOCTL_DEVICE_START);
		DEV_IOCTL_1_PARAMS(uart_clock, CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(uart_clock, CLK_IOCTL_ENABLE);

		SYS_ResetModule(uart_module_rst);

		UART_Open(uart_regs, cfg_hndl->baud_rate);

		/* Enable UART RDA/RLS/Time-out interrupt */
		UART_EnableInt(uart_regs,
				(UART_INTEN_RDAIEN_Msk  | UART_INTEN_RXTOIEN_Msk));

		irq_register_device_on_interrupt(uart_irq, adev);
		irq_set_priority(uart_irq, INTERRUPT_PRIORITY_FOR_UART );
		irq_enable_interrupt(uart_irq);
		i9xxx_uart_runtime->init_done = 1;
		break;

	case IOCTL_UART_DISABLE_TX :
		UART_DISABLE_INT(uart_regs,  UART_INTEN_TXENDIEN_Msk);
		i9xxx_uart_runtime->send_data_len = 0;
		break;

	case IOCTL_UART_ENABLE_TX :
		break;

	case IOCTL_UART_SET_ISR_CALLBACK_TX_DEV:
		cfg_hndl->callback_tx_dev =(struct dev_desc_t *) aIoctl_param1;
		break;

	case IOCTL_UART_SET_ISR_CALLBACK_RX_DEV:
		cfg_hndl->callback_rx_dev =(struct dev_desc_t *) aIoctl_param1;
		break;

	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                  uart_i9xxxx
#define MODULE_IOCTL_FUNCTION        uart_i9xxxx_ioctl
#define MODULE_CALLBACK_FUNCTION     uart_i9xxxx_callback
#define MODULE_PWRITE_FUNCTION       uart_i9xxxx_pwrite
#include "add_module.h"
