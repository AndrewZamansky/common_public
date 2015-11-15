/*
 * file : SW_UART_WRAPPER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _SW_UART_WRAPPER_H
#define _SW_UART_WRAPPER_H

#include "sw_uart_wrapper_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_sw_uart_wrapper_prerequirements_check.h" // should be after {sw_uart_wrapper_config.h,dev_managment_api.h}


/***************   typedefs    *******************/




#if SW_UART_WRAPPER_CONFIG_MAX_TX_BUFFER_SIZE <= (1<<8)
	typedef uint8_t tx_int_size_t;
#else
	#if SW_UART_WRAPPER_CONFIG_MAX_TX_BUFFER_SIZE <= (1<<16)
		typedef uint16_t tx_int_size_t;
	#else
		typedef uint32_t tx_int_size_t;
	#endif
#endif


#if SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE <= (1<<8)
	typedef uint8_t rx_int_size_t;
#else
	#if SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE <= (1<<16)
		typedef uint16_t rx_int_size_t;
	#else
		typedef uint32_t rx_int_size_t;
	#endif
#endif

typedef struct {

	pdev_descriptor_const   server_dev;

	os_queue_t xQueue ;
	uint8_t use_task_for_out ;

#if SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE > 0

	pdev_descriptor_const   this_dev;
	pdev_descriptor_const   client_dev;

	uint8_t *rx_buff;

	rx_int_size_t rx_buff_size;
	rx_int_size_t WritePos;
	rx_int_size_t ReadPos;


    uint8_t isDataInUse;
	uint8_t	bufferWasOverflowed;
#endif

	const uint8_t  *sendData;
	tx_int_size_t data_length;

	uint8_t sendDoneFlag;
} SW_UART_WRAPPER_Instance_t;




#endif /* */
