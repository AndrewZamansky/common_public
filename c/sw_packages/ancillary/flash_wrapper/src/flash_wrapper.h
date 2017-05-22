/*
 * file : flash_wrapper.h
 *
 *
 */

#ifndef _flash_wrapper_H
#define _flash_wrapper_H

struct flash_wrapper_cfg_t {
	struct dev_desc_t *   hw_flash_dev;
};

struct flash_wrapper_runtime_t {
	uint8_t 	*write_buffer;
	uint32_t	erase_block_size;
	uint32_t	flash_size;
	uint32_t	curr_block_addr;
	uint8_t		curr_block_is_dirty;
} ;

#endif /* */
