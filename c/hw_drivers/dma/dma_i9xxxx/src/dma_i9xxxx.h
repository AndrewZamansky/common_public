/*
 *
 * dma_i9xxxx.h
 *
 */

#ifndef _DMA_I9XXXX_H_
#define _DMA_I9XXXX_H_

#include "dev_management_api.h"

struct dma_i9xxxx_runtime_t {
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

SET_RUNTIME_DATA_TYPE(dma_i9xxxx, struct dma_i9xxxx_runtime_t);

#endif
