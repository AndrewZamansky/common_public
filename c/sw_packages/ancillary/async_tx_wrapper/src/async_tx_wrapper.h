/*
 * file : ASYNC_TX_WRAPPER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _ASYNC_TX_WRAPPER_H
#define _ASYNC_TX_WRAPPER_H

#include "src/_async_tx_wrapper_prerequirements_check.h"


/***************   typedefs    *******************/




#if CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE <= (1<<8)
	typedef uint8_t tx_int_size_t;
#else
	#if CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE <= (1<<16)
		typedef uint16_t tx_int_size_t;
	#else
		typedef uint32_t tx_int_size_t;
	#endif
#endif

typedef struct {
	pdev_descriptor_t   server_dev;
} async_tx_wrapper_instance_t;


typedef struct {
	os_queue_t xQueue ;
	const uint8_t  *sendData;
	tx_int_size_t data_length;
	os_queue_t xTX_WaitQueue ;

} async_tx_wrapper_runtime_instance_t;

#endif /* */
