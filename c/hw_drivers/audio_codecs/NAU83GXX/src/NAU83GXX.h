/*
 *
 * file :   NAU83GXX.h
 *
 *
 */

#ifndef _NAU83GXX_H
#define _NAU83GXX_H

#ifndef FOR_EXPORT
	#include "_project.h"
#else
	#include "hal.h"
#endif
#include "dev_management_api.h"
#include "os_wrapper.h"

#define NAU83GXX_REG_DEVICE_ID  0x46

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
	uint8_t chip_type;
};
SET_RUNTIME_DATA_TYPE(NAU83GXX, struct NAU83GXX_runtime_t);

#endif
