/*
 * file : memory_pool.h
 *
 */

#ifndef _memory_pool_H
#define _memory_pool_H

struct mem_pool_chunck_t
{
	void *mem;
	struct mem_pool_chunck_t *next_pool_chunks;
	struct mem_pool_chuncks_of_one_size_t *chuncks_of_one_size;
};


struct mem_pool_chuncks_of_one_size_t
{
	size_t   total_num_of_chunks;
	size_t   available_num_of_chunks;
	size_t   size_of_chunk;
	struct mem_pool_chunck_t *pool_chunks ;
	struct mem_pool_chuncks_of_one_size_t *next_chuncks_of_one_size ;
};


struct mem_pool_t
{
	struct mem_pool_chuncks_of_one_size_t *chuncks_of_one_size;
};

#endif
