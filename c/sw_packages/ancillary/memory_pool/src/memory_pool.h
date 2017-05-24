/*
 * file : memory_pool.h
 *
 *
 *
 */

#ifndef _memory_pool_H
#define _memory_pool_H


/***************   typedefs    *******************/

struct mem_pool_chunck_t
{

	void *mem;
	uint8_t inUse;

};


struct mem_pool_t
{
	size_t   num_of_chunks;
	size_t   size_of_chunk;
	struct mem_pool_chunck_t *pool_chunks ;
};



#endif /* */
