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

#include "dev_management_api.h"
#include "os_wrapper.h"


#if defined(CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC)
	typedef size_t tx_int_size_t;
#else
	#if CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE <= (1<<8)
		typedef uint8_t tx_int_size_t;
	#else
		#if CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE <= (1<<16)
			typedef uint16_t tx_int_size_t;
		#else
			typedef size_t tx_int_size_t;
		#endif
	#endif
#endif


struct async_tx_wrapper_runtime_t {
	os_queue_t xQueue ;
	const uint8_t  *sendData;
	tx_int_size_t data_length;
	os_queue_t xTX_WaitQueue ;

};

SET_RUNTIME_DATA_TYPE(async_tx_wrapper, struct async_tx_wrapper_runtime_t);

#endif
