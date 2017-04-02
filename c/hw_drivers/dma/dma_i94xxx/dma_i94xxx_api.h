
#ifndef _DMA_I94XXX_API_H_
#define _DMA_I94XXX_API_H_

#include "src/_dma_i94xxx_prerequirements_check.h"

/*****************  defines  **************/

#define DMA_I94XXX_API_TYPE_SPI1_RX	PDMA_SPI1_RX
#define DMA_I94XXX_API_TYPE_SPI1_TX	PDMA_SPI1_TX
#define DMA_I94XXX_API_TYPE_SPI2_RX	PDMA_SPI2_RX
#define DMA_I94XXX_API_TYPE_SPI2_TX	PDMA_SPI2_TX

/**********  define API  types ************/


/**********  define API  functions  ************/
typedef enum
{
	DMA_I94XXX_IOCTL_GET_RX_BUFF = IOCTL_LAST_COMMON_IOCTL+1,
	DMA_I94XXX_IOCTL_SET_TX_BUFF
}DMA_I94XXX_API_ioctl_t;

#endif
