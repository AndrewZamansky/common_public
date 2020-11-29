
#ifndef _cyclic_buffer_api_h_
#define _cyclic_buffer_api_h_

#include "stdint.h"

typedef void* cyclic_buffer_t;

enum CYCLIC_BUFFER_ERRORS {
	CYCLIC_BUFFER_NO_ERRORS,
	CYCLIC_BUFFER_ERR_BAD_BUFFER_SIZE,
	CYCLIC_BUFFER_ERR_NOT_INITIALIZED,
};

/*
 * max_num_of_items_in_buffer: must be power of 2 : 2, 4, 8, ..., 128, 256,..
 */
uint8_t cyclic_buffer_init(size_t item_size,
						size_t max_num_of_items_in_buffer,
						cyclic_buffer_t *new_cyclic_buffer);
void cyclic_buffer_delete(cyclic_buffer_t cyclic_buffer_handle);

size_t cyclic_buffer_add_items(cyclic_buffer_t cyclic_buffer_handle,
						const void *new_items_buff, size_t num_of_new_items);
size_t cyclic_buffer_get_items(cyclic_buffer_t cyclic_buffer_handle,
						void *get_items_buff, size_t num_of_get_items);

size_t cyclic_buffer_get_num_of_items(cyclic_buffer_t cyclic_buffer_handle);

#endif
