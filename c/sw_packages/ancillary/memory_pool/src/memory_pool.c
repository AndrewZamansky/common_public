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
#include "os_wrapper.h"



struct mem_pool_chuncks_of_one_size_t * get_chunks_of_one_size(
							struct mem_pool_t* mem_pool, size_t size_of_chunk)
{
	struct mem_pool_chuncks_of_one_size_t *chuncks_of_one_size;
	chuncks_of_one_size = mem_pool->chuncks_of_one_size;

	while (NULL != chuncks_of_one_size)
	{
		if (size_of_chunk == chuncks_of_one_size->size_of_chunk)
		{
			return chuncks_of_one_size;
		}
		chuncks_of_one_size = chuncks_of_one_size->next_chuncks_of_one_size;
	}
	chuncks_of_one_size =
			(struct mem_pool_chuncks_of_one_size_t *) os_safe_malloc(
							sizeof(struct mem_pool_chuncks_of_one_size_t));
	errors_api_check_if_malloc_succeed(chuncks_of_one_size);

	chuncks_of_one_size->next_chuncks_of_one_size =
								mem_pool->chuncks_of_one_size;
	chuncks_of_one_size->total_num_of_chunks = 0;
	chuncks_of_one_size->available_num_of_chunks = 0;
	chuncks_of_one_size->size_of_chunk = size_of_chunk;
	chuncks_of_one_size->pool_chunks = NULL;
	mem_pool->chuncks_of_one_size = chuncks_of_one_size;
	return chuncks_of_one_size;
}


/**
 * memory_pool_malloc()
 *
 * return:
 */
void *memory_pool_malloc(void *memory_pool_handle, size_t size_of_chunk)
{
	struct mem_pool_chuncks_of_one_size_t *chuncks_of_one_size;
	struct mem_pool_chunck_t *pool_chunk;
	void *mem;

	chuncks_of_one_size =
			get_chunks_of_one_size(memory_pool_handle, size_of_chunk);

	if (chuncks_of_one_size->available_num_of_chunks)
	{
		pool_chunk = chuncks_of_one_size->pool_chunks;
		mem = pool_chunk->mem;
		chuncks_of_one_size->pool_chunks = pool_chunk->next_pool_chunks;
		chuncks_of_one_size->available_num_of_chunks--;
		return mem;
	}

	//no free blocks if we got here

	// for fast 'free' put chunk info before memory block
	mem = os_safe_malloc(
		chuncks_of_one_size->size_of_chunk + sizeof(struct mem_pool_chunck_t));
	errors_api_check_if_malloc_succeed(mem);
	pool_chunk = (struct mem_pool_chunck_t *)mem;
	mem = ((uint8_t *)mem) + sizeof(struct mem_pool_chunck_t);
	pool_chunk->mem = mem;
	pool_chunk->chuncks_of_one_size = chuncks_of_one_size;
	chuncks_of_one_size->total_num_of_chunks++;

	return mem;
}


/**
 * memory_pool_zmalloc()
 *
 * return:
 */
void *memory_pool_zmalloc(void *memory_pool_handle, size_t size_of_chunk)
{
	void *mem;

	mem = memory_pool_malloc(memory_pool_handle, size_of_chunk);
	if (NULL != mem)
	{
		memset(mem, 0, size_of_chunk);
	}

	return mem;
}


/**
 * memory_pool_free()
 *
 * return:
 */
void memory_pool_free(void *mem)
{
	struct mem_pool_chunck_t *pool_chunk;
	struct mem_pool_chuncks_of_one_size_t *chuncks_of_one_size;

	pool_chunk = (struct mem_pool_chunck_t *)((uint8_t*)mem -
									sizeof(struct mem_pool_chunck_t));
	chuncks_of_one_size = pool_chunk->chuncks_of_one_size;
	pool_chunk->next_pool_chunks = chuncks_of_one_size->pool_chunks;
	chuncks_of_one_size->pool_chunks = pool_chunk;
	chuncks_of_one_size->available_num_of_chunks++;
}


/**
 * memory_pool_init()
 *
 * return:
 */
void *memory_pool_init(void)
{
	struct mem_pool_t *pInstance;

	pInstance = (struct mem_pool_t *)os_safe_malloc(sizeof(struct mem_pool_t));
	errors_api_check_if_malloc_succeed(pInstance);

	pInstance->chuncks_of_one_size = NULL;

	return pInstance ;
}


/**
 * memory_pool_delete()
 *
 * return:
 */
void memory_pool_delete(void *memory_pool_handle)
{
	struct mem_pool_chuncks_of_one_size_t *chuncks_of_one_size;
	struct mem_pool_chuncks_of_one_size_t *next_chuncks_of_one_size;
	struct mem_pool_chunck_t *pool_chunk;
	struct mem_pool_t* mem_pool;
	size_t available_num_of_chunks;

	mem_pool = (struct mem_pool_t*)memory_pool_handle;
	chuncks_of_one_size = mem_pool->chuncks_of_one_size;
	while (NULL != chuncks_of_one_size)
	{
		available_num_of_chunks = chuncks_of_one_size->available_num_of_chunks;
		if (available_num_of_chunks != chuncks_of_one_size->total_num_of_chunks)
		{
			CRITICAL_ERROR("all chunks should be free before closing mem pool");
		}

		pool_chunk = chuncks_of_one_size->pool_chunks;
		while (available_num_of_chunks--)
		{
			struct mem_pool_chunck_t *tmp_pool_chunk;
			tmp_pool_chunk = pool_chunk;
			pool_chunk = pool_chunk->next_pool_chunks;
			os_safe_free(tmp_pool_chunk);
		}
		next_chuncks_of_one_size =
				chuncks_of_one_size->next_chuncks_of_one_size;
		os_safe_free(chuncks_of_one_size);
		chuncks_of_one_size = next_chuncks_of_one_size;
	}
	os_safe_free(mem_pool);
}
