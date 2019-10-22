/*
 * file : ASYNC_RX_WRAPPER.h
 *
 *
 */

#ifndef _ASYNC_RX_WRAPPER_H
#define _ASYNC_RX_WRAPPER_H

#include "_project_defines.h"
#include "dev_management_api.h"

struct async_rx_wrapper_runtime_t {

	rx_int_size_t WritePos;
	rx_int_size_t ReadPos;
	uint8_t isDataInUse;
	uint8_t bufferWasOverflowed;

};

SET_RUNTIME_DATA_TYPE(async_rx_wrapper, struct async_rx_wrapper_runtime_t);

#endif
