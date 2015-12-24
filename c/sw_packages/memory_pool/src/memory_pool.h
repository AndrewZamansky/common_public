/*
 * file : memory_pool.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _memory_pool_H
#define _memory_pool_H

#include "memory_pool_config.h"
#include "_memory_pool_prerequirements_check.h" // should be after dev_managment_config.h

/***************   typedefs    *******************/

typedef struct
{

	void *mem;
	uint8_t inUse;

} xMemPool_t;


typedef struct
{

	size_t num_of_chunks ;
	xMemPool_t *pool_chunks ;

} MEMORY_POOL_Instance_t;



#endif /* */
