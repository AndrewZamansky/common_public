
#ifndef _memory_pool_api_h_
#define _memory_pool_api_h_


#include "stdint.h"

/**********  define API  types ************/


/**********  define API  functions  ************/



/**********  define API  functions  ************/

void *memory_pool_init(size_t size_of_chunk);
void memory_pool_delete(void *memory_pool_handle);

void *memory_pool_malloc(void *memory_pool_handle);
void *memory_pool_zmalloc(void *memory_pool_handle);
void memory_pool_free(void *memory_pool_handle , void *mem);
size_t memory_pool_get_chunk_size(void *memory_pool_handle);


#endif
