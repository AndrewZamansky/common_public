/*
 *
 * dma_i94xxx.h
 *
 */

#ifndef _DMA_I94XXX_H_
#define _DMA_I94XXX_H_

/********  includes *********************/

#include "dev_management_api.h"


/********  types  *********************/


struct dma_i94xxx_runtime_t {
	uint8_t   curr_dma_buff_indx;
	uint8_t   next_supplied_tx_buffer;
	uint8_t   next_supplied_rx_buffer;
	uint8_t   **buff;
	uint8_t   *buff_status;
	uint8_t   dma_peripheral_direction;
	uint32_t  buff_size_in_transfer_words;
	uint8_t   needed_full_dma_start;
	uint8_t   prefilled_buffers;

};

#endif
