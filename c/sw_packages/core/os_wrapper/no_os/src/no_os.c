/*
 * no_os.c
 *
 */

#include "_project_defines.h"
#include "errors_api.h"
#include "os_wrapper.h"

struct no_os_queue {
	size_t num_of_elements;
	size_t size_of_element;
	size_t curr_write_pos; // next postion to write data to
	size_t curr_read_pos; // last position that was read
	uint8_t *mem;
};



uint8_t os_queue_send_without_wait(os_queue_t queue,  void * pData)
{
	struct no_os_queue  *p_queue;
	uint8_t *mem;
	size_t offset;
	size_t curr_write_pos;
	size_t curr_read_pos;
	size_t num_of_elements;
	size_t size_of_element;

	p_queue = (struct no_os_queue  *) queue;

	num_of_elements = p_queue->num_of_elements;
	curr_write_pos = p_queue->curr_write_pos;
	curr_read_pos = p_queue->curr_read_pos;
	if (curr_write_pos == curr_read_pos) return OS_QUEUE_SEND_FAILED; // full

	size_of_element = p_queue->size_of_element;
	offset = curr_write_pos * size_of_element;
	mem = p_queue->mem;
	mem += offset;
	memcpy(mem, (uint8_t*)pData, size_of_element);

	curr_write_pos++;
	if (curr_write_pos >= num_of_elements) curr_write_pos = 0;

	p_queue->curr_write_pos = curr_write_pos; //should be at the end of function

	return OS_QUEUE_SEND_SUCCESS;
}


uint8_t os_queue_receive_with_timeout(
		os_queue_t queue, void *pData, uint32_t timeout)
{
	struct no_os_queue  *p_queue;
	uint8_t *mem;
	size_t offset;
	size_t curr_write_pos;
	size_t curr_read_pos;
	size_t num_of_elements;
	size_t size_of_element;

	if (0 != timeout)
	{
		CRITICAL_ERROR("non zero timeout not implemented yet");
	}

	p_queue = (struct no_os_queue  *) queue;

	num_of_elements = p_queue->num_of_elements;
	curr_read_pos = p_queue->curr_read_pos;
	curr_read_pos++;
	if (curr_read_pos == num_of_elements) curr_read_pos = 0;

	curr_write_pos = p_queue->curr_write_pos;
	if (curr_read_pos == curr_write_pos) return OS_QUEUE_RECEIVE_FAILED; //empty

	size_of_element = p_queue->size_of_element;
	offset = curr_read_pos * size_of_element;
	mem = p_queue->mem;
	mem += offset;
	memcpy((uint8_t*)pData, mem, size_of_element);

	p_queue->curr_read_pos = curr_read_pos; //should be at the end of function

	return OS_QUEUE_RECEIVE_SUCCESS;
}


void *no_os_create_queque(size_t num_of_elements, size_t size_of_element)
{
	struct no_os_queue  *p_queue;
	uint8_t *mem;

	p_queue = (struct no_os_queue *)malloc(sizeof(struct no_os_queue));
	errors_api_check_if_malloc_succeed(p_queue);

	num_of_elements++;// additional element to for easier detection
	                  // of full array. write_pos changed only during
					  // sending, read_pos change only during receiving
	p_queue->num_of_elements = num_of_elements;
	p_queue->size_of_element = size_of_element;
	p_queue->curr_write_pos = 1;
	p_queue->curr_read_pos = 0;
	mem = (uint8_t *)malloc(num_of_elements * size_of_element);
	errors_api_check_if_malloc_succeed(mem);
	p_queue->mem = mem;

	return p_queue;
}

