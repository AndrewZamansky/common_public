/*
 *
 * dma_i94xxx.h
 *
 *
 *
 *
 *
 */

#ifndef _DMA_I94XXX_H_
#define _DMA_I94XXX_H_

/********  includes *********************/

#include "src/_dma_i94xxx_prerequirements_check.h"

/********  types  *********************/

typedef struct {
	pdev_descriptor_t   callback_dev;
	uint8_t   channel_num;
	uint8_t   peripheral_type;
	uint32_t   buff_size;
} dma_i94xxx_instance_t;

typedef struct {
	uint8_t	curr_buff_indx;
	uint8_t	*buff[2];
	uint8_t	buff_status[2];
	uint8_t   dma_peripheral_direction;
} dma_i94xxx_runtime_instance_t;

#endif
