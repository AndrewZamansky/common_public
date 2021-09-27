/*
 *
 * file :   85L40.c
 *
 *
 */

#ifndef _83G10_H
#define _83G10_H

#include "_project.h"
#include "dev_management_api.h"

//#define DEBUG
#include "PRINTF_api.h"

#include "os_wrapper.h"

struct NAU83GXX_runtime_t {
	os_queue_t msg_queue;
	os_queue_t wait_for_finish_queue;
	os_mutex_t main_mutex;
	os_task_t  task_handle;
	uint16_t dataSetBufPos;
	uint16_t addr_offset;
	uint16_t sendLen;
	uint8_t *dataBuf;
	uint8_t  state;
	uint8_t  do_recovery;
};
SET_RUNTIME_DATA_TYPE(NAU83GXX, struct NAU83GXX_runtime_t);

#endif
