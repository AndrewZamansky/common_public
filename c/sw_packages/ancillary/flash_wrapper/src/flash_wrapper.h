/*
 * file : flash_wrapper.h
 *
 *
 */

#ifndef _flash_wrapper_H
#define _flash_wrapper_H

#include "dev_management_api.h"

struct flash_wrapper_runtime_t {
	uint8_t  *write_buffer;
	uint32_t  erase_block_size;
	uint32_t  flash_size;
	uint32_t  curr_block_addr;
	uint8_t   curr_block_is_dirty;
};
SET_RUNTIME_DATA_TYPE(flash_wrapper, struct flash_wrapper_runtime_t);

#endif
