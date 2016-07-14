
#ifndef _memory_pool_api_h_
#define _memory_pool_api_h_


#include "src/_memory_pool_prerequirements_check.h"// should be after dev_management_config.h

/**********  define API  types ************/


/**********  define API  functions  ************/



/**********  define API  functions  ************/

void *memory_pool_init(size_t num_of_chunks,size_t size_of_chunk);

void *memory_pool_malloc(void *memory_pool_handle);
void memory_pool_free(void *memory_pool_handle , void *mem);


#endif
