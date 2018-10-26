/*
 *
 *   file  :  "memory_pool.c"
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "memory_pool_api.h"

#include "memory_pool.h"


/***************   defines    *******************/

/***************   typedefs    *******************/


/**********   external variables    **************/

/***********   local variables    **************/


/**
 * memory_pool_malloc()
 *
 * return:
 */
void *memory_pool_malloc(void *memory_pool_handle)
{
	size_t	i;
	size_t	num_of_chunks;
	struct mem_pool_chunck_t *pMemPool;
	struct mem_pool_t* mem_pool;
	void *mem;
	struct mem_pool_chunck_t *pool_chunks;
	struct mem_pool_chunck_t *pool_chunk;

	mem_pool = (struct mem_pool_t*)memory_pool_handle;
	num_of_chunks = mem_pool->num_of_chunks;
	pMemPool = mem_pool->pool_chunks;
	i = num_of_chunks;
	while( i--)
	{
		if(0 == pMemPool->inUse)
		{
			pMemPool->inUse = 1;
			return pMemPool->mem;
		}
		pMemPool++;
	}

	//no free blocks if we got here
	num_of_chunks++;
	mem_pool->num_of_chunks = num_of_chunks;
	pool_chunks = mem_pool->pool_chunks;
	pool_chunks = (struct mem_pool_chunck_t *)realloc(pool_chunks,
					num_of_chunks * sizeof(struct mem_pool_chunck_t));
	if (NULL == pool_chunks)
	{
		CRITICAL_ERROR("not enough memory in heap");
	}
	mem_pool->pool_chunks = pool_chunks;
	pool_chunk = &pool_chunks[num_of_chunks - 1];
	pool_chunk->inUse = 1;
	mem = malloc(mem_pool->size_of_chunk);
	pool_chunk->mem = mem;
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
	size_t	num_of_chunks;
	struct mem_pool_chunck_t *pMemPool;

	num_of_chunks = ((struct mem_pool_t*)memory_pool_handle)->num_of_chunks;
	pMemPool = ((struct mem_pool_t*)memory_pool_handle)->pool_chunks;
	while( num_of_chunks--)
	{
		if(mem == pMemPool->mem)
		{
			pMemPool->inUse = 0;
			return ;
		}
		pMemPool++;
	}

	//error if we got here
	CRITICAL_ERROR("bad memory chunk");
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
	if(NULL == pInstance) return NULL;

	pInstance->num_of_chunks = 0;
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
	size_t	num_of_chunks;
	struct mem_pool_chunck_t *pMemPool;

	num_of_chunks = ((struct mem_pool_t*)memory_pool_handle)->num_of_chunks;
	pool_chunk = ((struct mem_pool_t*)memory_pool_handle)->pool_chunks;
	pMemPool = pool_chunk;
	while( num_of_chunks--)
	{
		free(pMemPool->mem);
		pMemPool++;
	}
	free(pool_chunk);
	free(memory_pool_handle);
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
