/*
 *
 *   file  :  "memory_pool.c"
 *
 */



/***************   includes    *******************/
#include "memory_pool_api.h" //place first to test that header file is self-contained

#include "_memory_pool_prerequirements_check.h"

#include "memory_pool.h"


/***************   defines    *******************/
#define INSTANCE(hndl)	((MEMORY_POOL_Instance_t*)hndl)

/***************   typedefs    *******************/


/**********   external variables    **************/

/***********   local variables    **************/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        memory_pool_malloc                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void *memory_pool_malloc(void *memory_pool_handle)
{
	size_t	i = INSTANCE(memory_pool_handle)->num_of_chunks;
	xMemPool_t *pMemPool = INSTANCE(memory_pool_handle)->pool_chunks;
	while( i--)
	{
		if(0 == pMemPool->inUse)
		{
			pMemPool->inUse = 1;
			return pMemPool->mem;
		}
		pMemPool++;
	}
	while (1); // debug trap
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        memory_pool_free                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void memory_pool_free(void *memory_pool_handle , void *mem)
{
	size_t	i = INSTANCE(memory_pool_handle)->num_of_chunks;
	xMemPool_t *pMemPool = INSTANCE(memory_pool_handle)->pool_chunks;
	while( i--)
	{
		if(mem == pMemPool->mem)
		{
			pMemPool->inUse = 0;
			return ;
		}
		pMemPool++;
	}
	while (1); // debug trap
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        memory_pool_init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void *memory_pool_init(size_t num_of_chunks,size_t size_of_chunk)
{
	MEMORY_POOL_Instance_t *pInstance;
	xMemPool_t *pool_chunk;


	pInstance = (MEMORY_POOL_Instance_t *)malloc(sizeof(MEMORY_POOL_Instance_t));
	if(NULL == pInstance) return NULL;

	pool_chunk = (xMemPool_t *)malloc(num_of_chunks*sizeof(xMemPool_t));
	pInstance->num_of_chunks = num_of_chunks;
	pInstance->pool_chunks = pool_chunk;

	while(num_of_chunks--)
	{
		pool_chunk->inUse = 0 ;
		pool_chunk->mem = malloc(size_of_chunk);
		pool_chunk++;
	}

	return pInstance ;
}
