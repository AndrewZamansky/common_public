/*
 *
 *   file  :  "memory_pool.c"
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"
#include "errors_api.h"

#include "memory_pool_api.h"

#include "memory_pool.h"


/**
 * memory_pool_malloc()
 *
 * return:
 */
void *memory_pool_malloc(void *memory_pool_handle)
{
	struct mem_pool_t* mem_pool;
	struct mem_pool_chunck_t *pool_chunk;
	void *mem;

	mem_pool = (struct mem_pool_t*)memory_pool_handle;
	if (mem_pool->available_num_of_chunks)
	{
		pool_chunk = mem_pool->pool_chunks;
		mem = pool_chunk->mem;
		mem_pool->pool_chunks = pool_chunk->next_pool_chunks;
		mem_pool->available_num_of_chunks--;
		return mem;
	}

	//no free blocks if we got here

	// for fast 'free' put chunk info before memory block
	mem = malloc(mem_pool->size_of_chunk + sizeof(struct mem_pool_chunck_t));
	errors_api_check_if_malloc_succeed(mem);
	pool_chunk = (struct mem_pool_chunck_t *)mem;
	mem = ((uint8_t *)mem) + sizeof(struct mem_pool_chunck_t);
	pool_chunk->mem = mem;
	mem_pool->total_num_of_chunks++;

	return mem;
}


/**
 * memory_pool_zmalloc()
 *
 * return:
 */
void *memory_pool_zmalloc(void *memory_pool_handle)
{
	void *mem;

	mem = memory_pool_malloc(memory_pool_handle);
	if (NULL != mem)
	{
		size_t	size_of_chunk;
		size_of_chunk = ((struct mem_pool_t*)memory_pool_handle)->size_of_chunk;
		memset(mem, 0, size_of_chunk);
	}

	return mem;
}


/**
 * memory_pool_free()
 *
 * return:
 */
void memory_pool_free(void *memory_pool_handle, void *mem)
{
	struct mem_pool_t* mem_pool;
	struct mem_pool_chunck_t *pool_chunk;

	mem_pool = (struct mem_pool_t*)memory_pool_handle;
	pool_chunk = (struct mem_pool_chunck_t *)((uint8_t*)mem -
									sizeof(struct mem_pool_chunck_t));
	pool_chunk->next_pool_chunks = mem_pool->pool_chunks;
	mem_pool->pool_chunks = pool_chunk;
	mem_pool->available_num_of_chunks++;
}



/**
 * memory_pool_init()
 *
 * return:
 */
void *memory_pool_init(size_t size_of_chunk)
{
	struct mem_pool_t *pInstance;

	pInstance = (struct mem_pool_t *)malloc(sizeof(struct mem_pool_t));
	errors_api_check_if_malloc_succeed(pInstance);

	pInstance->total_num_of_chunks = 0;
	pInstance->available_num_of_chunks = 0;
	pInstance->size_of_chunk = size_of_chunk;
	pInstance->pool_chunks = NULL;

	return pInstance ;
}


/**
 * memory_pool_delete()
 *
 * return:
 */
void memory_pool_delete(void *memory_pool_handle)
{
	struct mem_pool_chunck_t *pool_chunk;
	struct mem_pool_t* mem_pool;
	size_t available_num_of_chunks;

	mem_pool = (struct mem_pool_t*)memory_pool_handle;
	available_num_of_chunks = mem_pool->available_num_of_chunks;
	if (available_num_of_chunks != mem_pool->total_num_of_chunks)
	{
		CRITICAL_ERROR("all chunks should be free before closing mem pool");
	}

	pool_chunk = mem_pool->pool_chunks;
	while (available_num_of_chunks--)
	{
		struct mem_pool_chunck_t *tmp_pool_chunk;
		tmp_pool_chunk = pool_chunk;
		pool_chunk = pool_chunk->next_pool_chunks;
		free(tmp_pool_chunk);
	}

	free(mem_pool);
}

/**
 * memory_pool_get_chunk_size()
 *
 * return:
 */
size_t memory_pool_get_chunk_size(void *memory_pool_handle)
{
	struct mem_pool_t *mem_pool;

	mem_pool = (struct mem_pool_t*)memory_pool_handle;
	if (NULL == mem_pool)
	{
		CRITICAL_ERROR("uninitialized memory pool");
	}

	return mem_pool->size_of_chunk;
}
