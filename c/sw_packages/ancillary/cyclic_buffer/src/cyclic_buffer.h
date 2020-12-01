/*
 * file : cyclic_buffer.h
 *
 */

#ifndef _cyclic_buffer_H
#define _cyclic_buffer_H

struct cyclic_buffer_runtime_t
{
	uint8_t *mem;
	size_t  max_num_of_items_in_buffer;
	size_t  wrap_mask;
	size_t  write_pointer; // points to next(empty) element that will be written
	size_t  read_pointer; // points to last elements that was read
	size_t  valid_items_in_buffer;
	size_t item_size;
	uint16_t  init_done;
};

#endif
