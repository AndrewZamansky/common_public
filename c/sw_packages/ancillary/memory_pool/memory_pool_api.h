
#ifndef _memory_pool_api_h_
#define _memory_pool_api_h_

#include "stdint.h"


void *memory_pool_init(void);
void memory_pool_delete(void *memory_pool_handle);

void *memory_pool_malloc(void *memory_pool_handle, size_t size_of_chunk);
void *memory_pool_zmalloc(void *memory_pool_handle, size_t size_of_chunk);
void memory_pool_free(void *mem);


#endif
