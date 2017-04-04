/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "dma_i94xxx_api.h"
#include 	"src/dma_i94xxx.h"

uint8_t dma_i94xxx_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME							dma_i94xxx
#define	MODULE_IOCTL_FUNCTION				dma_i94xxx_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE		dma_i94xxx_instance_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE		dma_i94xxx_runtime_instance_t

#ifdef DT_DEV_MODULE

	#ifndef DMA_I94XXX_DT_CALLBACK_PDEV
		#error "DMA_I94XXX_DT_CALLBACK_PDEV should be defined"
	#endif

	#ifndef DMA_I94XXX_DT_CHANNEL_NUM
		#error "DMA_I94XXX_DT_CHANNEL_NUM should be defined"
	#endif


	#ifndef DMA_I94XXX_DT_PERIPHERAL_TYPE
		#error "DMA_I94XXX_DT_PERIPHERAL_TYPE should be defined"
	#endif


	#ifndef DMA_I94XXX_DT_TRANSFER_WORD_SIZE
		#error "DMA_I94XXX_DT_TRANSFER_WORD_SIZE should be defined"
	#endif

	#ifndef DMA_I94XXX_DT_BUFFER_SIZE
		#error "DMA_I94XXX_DT_CHANNEL_NUM should be defined"
	#endif


	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(DMA_I94XXX_DT_CALLBACK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT			\
		{									\
			P_TO_STATIC_DEVICE_INST(DMA_I94XXX_DT_CALLBACK_PDEV) ,	\
			DMA_I94XXX_DT_CHANNEL_NUM ,			\
			DMA_I94XXX_DT_PERIPHERAL_TYPE ,		\
			DMA_I94XXX_DT_TRANSFER_WORD_SIZE,	\
			DMA_I94XXX_DT_BUFFER_SIZE 			\
		}

	#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef DMA_I94XXX_DT_CALLBACK_PDEV
#undef DMA_I94XXX_DT_CHANNEL_NUM
#undef DMA_I94XXX_DT_PERIPHERAL_TYPE
#undef DMA_I94XXX_DT_BUFFER_SIZE
