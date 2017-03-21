/*
 * file : ASYNC_RX_WRAPPER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _ASYNC_RX_WRAPPER_H
#define _ASYNC_RX_WRAPPER_H

#include "src/_async_rx_wrapper_prerequirements_check.h"


/***************   typedefs    *******************/
#define CONFIG_ASYNC_RX_WRAPPER_RX_BUFFER_SIZE  64

#if defined(CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC)
	typedef size_t rx_int_size_t;
#else
	#if CONFIG_ASYNC_RX_WRAPPER_RX_BUFFER_SIZE <= (1<<8)
		typedef uint8_t rx_int_size_t;
	#else
		#if CONFIG_ASYNC_RX_WRAPPER_RX_BUFFER_SIZE <= (1<<16)
			typedef uint16_t rx_int_size_t;
		#else
			typedef size_t rx_int_size_t;
		#endif
	#endif
#endif

typedef struct {

	pdev_descriptor_t   server_dev;
	pdev_descriptor_t   client_dev;
#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
	rx_int_size_t rx_buff_size;
	uint8_t *rx_buff;
#else
	uint8_t	rx_buff[CONFIG_ASYNC_RX_WRAPPER_RX_BUFFER_SIZE];
#endif

} async_rx_wrapper_instance_t;


typedef struct {

	rx_int_size_t WritePos;
	rx_int_size_t ReadPos;
    uint8_t isDataInUse;
	uint8_t	bufferWasOverflowed;

} async_rx_wrapper_runtime_instance_t;

#endif /* */
