
#ifndef _DMA_I9XXXX_API_H_
#define _DMA_I9XXXX_API_H_

#include "dev_management_api.h"
#include "pdma.h"

#if defined(CONFIG_I94XXX)
	#define DMA_I94XXX_API_TYPE_SPI1_RX  PDMA_SPI1_RX
	#define DMA_I94XXX_API_TYPE_SPI1_TX  PDMA_SPI1_TX
	#define DMA_I94XXX_API_TYPE_SPI2_RX  PDMA_SPI2_RX
	#define DMA_I94XXX_API_TYPE_SPI2_TX  PDMA_SPI2_TX
	#define DMA_I94XXX_API_TYPE_I2S_RX   PDMA_I2S0_RX
	#define DMA_I94XXX_API_TYPE_I2S_TX   PDMA_I2S0_TX
	#define DMA_I94XXX_API_TYPE_DPWM     PDMA_DPWM
	#define DMA_I94XXX_API_TYPE_DMIC     PDMA_DMIC_RX
#elif defined(CONFIG_I96XXX_M0)
	#define PDMA_DPWM  PDMA_DPWM_TX
	#define DMA_I96XXX_API_TYPE_I2S0_RX   PDMA_I2S0_RX
	#define DMA_I96XXX_API_TYPE_I2S0_TX   PDMA_I2S0_TX
	#define DMA_I96XXX_API_TYPE_I2S1_RX   PDMA_I2S1_RX
	#define DMA_I96XXX_API_TYPE_I2S1_TX   PDMA_I2S1_TX
	#define DMA_I96XXX_API_TYPE_SPI2_RX   PDMA_SPI2_RX
	#define DMA_I96XXX_API_TYPE_SPI2_TX   PDMA_SPI2_TX
	#define DMA_I96XXX_API_TYPE_DPWM      PDMA_DPWM
#endif


#define DMA_I9XXXX_API_TRANSFER_WORD_8BIT   PDMA_WIDTH_8
#define DMA_I9XXXX_API_TRANSFER_WORD_16BIT  PDMA_WIDTH_16
#define DMA_I9XXXX_API_TRANSFER_WORD_32BIT  PDMA_WIDTH_32

enum DMA_I9XXXX_API_ioctl_t {
	DMA_I9XXXX_IOCTL_GET_RX_BUFF = IOCTL_LAST_COMMON_IOCTL + 1,
	DMA_I9XXXX_IOCTL_RELEASE_RX_BUFF,
	DMA_I9XXXX_IOCTL_GET_EMPTY_TX_BUFF,
	DMA_I9XXXX_IOCTL_RELEASE_TX_BUFF,
	DMA_I9XXXX_IOCTL_GET_STATUS
};


struct dma_i9xxxx_get_status_t {
	uint32_t   tx_underflow;
	uint32_t   tx_underflow_threshold_reached;
	uint32_t   tx_overflow;
	uint32_t   tx_overflow_threshold_reached;
	uint32_t   rx_underflow;
	uint32_t   rx_overflow;
};


struct dma_i9xxxx_cfg_t {
	struct dev_desc_t *callback_dev;
	struct dev_desc_t *peripheral_dev;
	uint8_t    channel_num;
	uint8_t    peripheral_type;
	uint8_t    num_of_buffers;
	uint8_t    num_of_prefilled_buffer_before_tx_start;
	uint16_t   transfer_word_size;
	size_t     buff_size;

};
SET_CONFIG_TYPE(dma_i9xxxx, struct dma_i9xxxx_cfg_t);

#endif
