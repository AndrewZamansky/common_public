/*
 *
 *   file  :  "cyclic_buffer.c"
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"
#include "errors_api.h"
#include "cyclic_buffer_api.h"
#include "cyclic_buffer.h"
#include "os_wrapper.h"


size_t cyclic_buffer_add_items(cyclic_buffer_t cyclic_buffer_handle,
					const void *new_items_buff, size_t num_of_new_items)
{
	struct cyclic_buffer_runtime_t  *buffer_handle;
	size_t  write_pointer;
	size_t  read_pointer;
	size_t  items_to_copy;
	size_t  memsize_to_copy;
	size_t  max_num_of_items_in_buffer;
	uint8_t  *new_buff;
	uint8_t  *mem;
	size_t item_size;
	size_t  wrap_mask;
	size_t items_actually_copied;

	buffer_handle = (struct cyclic_buffer_runtime_t  *)cyclic_buffer_handle;
	write_pointer = buffer_handle->write_pointer;
	read_pointer = buffer_handle->read_pointer;
	item_size = buffer_handle->item_size;
	wrap_mask = buffer_handle->wrap_mask;
	mem = buffer_handle->mem;
	max_num_of_items_in_buffer = buffer_handle->max_num_of_items_in_buffer;
	new_buff = (uint8_t*)new_items_buff;

	items_actually_copied = 0;
	while (num_of_new_items)
	{
		if (write_pointer == read_pointer)
		{
			// always leave 1 item before read pointer in this way
			// write and read will never point to the same element
			return items_actually_copied;// overflow
		}
		if (write_pointer >= read_pointer)
		{
			if (0 == read_pointer)
			{
				return items_actually_copied;// overflow

			}
			items_to_copy = max_num_of_items_in_buffer - write_pointer;
		}
		else
		{
			items_to_copy = read_pointer - write_pointer;
		}

		if (items_to_copy > num_of_new_items)
		{
			items_to_copy = num_of_new_items;
		}

		items_actually_copied += items_to_copy;
		memsize_to_copy = item_size * items_to_copy;
		memcpy(mem + (write_pointer * item_size), new_buff, memsize_to_copy);
		new_buff += memsize_to_copy;
		num_of_new_items -= items_to_copy;
		write_pointer += items_to_copy;
		write_pointer &= wrap_mask; // wrap if we reach end of cyclic buffer
	}
	buffer_handle->write_pointer = write_pointer;
	return items_actually_copied;
}


size_t cyclic_buffer_get_items(cyclic_buffer_t cyclic_buffer_handle,
						void *get_items_buff, size_t num_of_get_items)
{
	struct cyclic_buffer_runtime_t  *buffer_handle;
	size_t  write_pointer;
	size_t  read_pointer;
	size_t  items_to_copy;
	size_t  memsize_to_copy;
	size_t  max_num_of_items_in_buffer;
	uint8_t  *get_buff;
	uint8_t  *mem;
	size_t item_size;
	size_t  wrap_mask;
	size_t items_actually_copied;

	buffer_handle = (struct cyclic_buffer_runtime_t  *)cyclic_buffer_handle;
	write_pointer = buffer_handle->write_pointer;
	read_pointer = buffer_handle->read_pointer;
	item_size = buffer_handle->item_size;
	wrap_mask = buffer_handle->wrap_mask;
	mem = buffer_handle->mem;
	max_num_of_items_in_buffer = buffer_handle->max_num_of_items_in_buffer;
	get_buff = (uint8_t*)get_items_buff;

	read_pointer++; // point to next valid item
	read_pointer &= wrap_mask; // wrap if we reach end of cyclic buffer
	items_actually_copied = 0;
	while (num_of_get_items)
	{
		if (write_pointer == read_pointer)
		{
			return items_actually_copied;// underflow
		}
		else if (write_pointer > read_pointer)
		{
			items_to_copy = write_pointer - read_pointer;
		}
		else
		{
			items_to_copy = max_num_of_items_in_buffer - read_pointer;
		}

		if (items_to_copy > num_of_get_items)
		{
			items_to_copy = num_of_get_items;
		}

		items_actually_copied += items_to_copy;
		memsize_to_copy = item_size * items_to_copy;
		memcpy(get_buff, mem + (read_pointer * item_size), memsize_to_copy);
		get_buff += memsize_to_copy;
		num_of_get_items -= items_to_copy;
		read_pointer += items_to_copy;
		read_pointer &= wrap_mask; // wrap if we reach end of cyclic buffer
	}

	read_pointer -= 1;// if read_pointer was 0 then -1 will give 0xffffffff
	read_pointer &= wrap_mask; // so we need to apply mask
	buffer_handle->read_pointer = read_pointer;
	return items_actually_copied;
}


size_t cyclic_buffer_get_num_of_items(cyclic_buffer_t cyclic_buffer_handle)
{
	struct cyclic_buffer_runtime_t  *buffer_handle;
	size_t  write_pointer;
	size_t  read_pointer;

	buffer_handle = (struct cyclic_buffer_runtime_t  *)cyclic_buffer_handle;
	write_pointer = buffer_handle->write_pointer;
	read_pointer = buffer_handle->read_pointer;

	if (write_pointer > read_pointer)
	{
		return write_pointer - read_pointer - 1;
	}
	else
	{
		return  buffer_handle->max_num_of_items_in_buffer -
							(read_pointer + 1 - write_pointer);
	}
}


uint8_t cyclic_buffer_init(size_t item_size,
						size_t max_num_of_items_in_buffer,
						cyclic_buffer_t *new_cyclic_buffer)
{
	struct cyclic_buffer_runtime_t  *cyclic_buffer_handle;
	uint8_t *mem;
	size_t wrap_mask;

	*new_cyclic_buffer = NULL;

	wrap_mask = max_num_of_items_in_buffer - 1;
	if ( (0 == max_num_of_items_in_buffer) ||
			(0 != (max_num_of_items_in_buffer & wrap_mask)))
	{
		return CYCLIC_BUFFER_ERR_BAD_BUFFER_SIZE;
	}

	cyclic_buffer_handle = (struct cyclic_buffer_runtime_t *)os_safe_malloc(
			sizeof(struct cyclic_buffer_runtime_t));
	errors_api_check_if_malloc_succeed(cyclic_buffer_handle);

	mem = (uint8_t*)os_safe_malloc(item_size * max_num_of_items_in_buffer);
	errors_api_check_if_malloc_succeed(mem);
	cyclic_buffer_handle->mem = mem;
	cyclic_buffer_handle->item_size = item_size;
	cyclic_buffer_handle->max_num_of_items_in_buffer =
								max_num_of_items_in_buffer;
	cyclic_buffer_handle->valid_items_in_buffer = 0;
	cyclic_buffer_handle->write_pointer = 0;
	cyclic_buffer_handle->read_pointer = max_num_of_items_in_buffer - 1;
	cyclic_buffer_handle->wrap_mask = wrap_mask;
	//cyclic_buffer_handle->empty = 1;

	*new_cyclic_buffer = cyclic_buffer_handle;
	return CYCLIC_BUFFER_NO_ERRORS;
}


void cyclic_buffer_delete(cyclic_buffer_t cyclic_buffer_handle)
{
	struct cyclic_buffer_runtime_t  *buffer_handle;

	buffer_handle = (struct cyclic_buffer_runtime_t  *)cyclic_buffer_handle;
	os_safe_free(buffer_handle->mem);
	os_safe_free(buffer_handle);
}
