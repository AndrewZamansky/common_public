/*
 *
 * dma_i94xxx.c

 *
 */

/********  includes *********************/

#include "src/_dma_i94xxx_prerequirements_check.h"

#include "dma_i94xxx.h"


#include "ISD94XXXSeries.h"

#include "irq_api.h"
#include "os_wrapper.h"


#include "dma_i94xxx_add_component.h"

/********  defines *********************/
#define DMA_TO_PERIPHERAL	0
#define DMA_FROM_PERIPHERAL	1

#define BUFF_IDLE			0
#define BUFF_IN_PROCCESS	1
#define BUFF_READY			2

#define writeRegU32(addr,val)	( (*(volatile uint32_t *)(addr)) = (val) )

/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/
#define MAX_NUMBER_OF_CHANNELS	16
pdev_descriptor_t channel_pdev[MAX_NUMBER_OF_CHANNELS] = {0};


static void update_buffer(dma_i94xxx_instance_t *cfg_hndl,
		dma_i94xxx_runtime_instance_t *runtime_hndl)
{
	uint8_t	curr_buff_indx;
	uint8_t	next_buff_indx;
	uint8_t *next_buff_status;
	uint8_t channel_num;
	uint8_t **buffers;

	channel_num = cfg_hndl->channel_num;
	curr_buff_indx = runtime_hndl->curr_buff_indx;
	next_buff_indx = 1 - curr_buff_indx;
	next_buff_status = &runtime_hndl->buff_status[next_buff_indx];
	buffers = runtime_hndl->buff;

	if (DMA_TO_PERIPHERAL == runtime_hndl->dma_peripheral_direction)
	{
		runtime_hndl->buff_status[curr_buff_indx] = BUFF_IDLE;
		if (BUFF_READY == *next_buff_status)
		{
			*next_buff_status = BUFF_IN_PROCCESS;
			writeRegU32((uint32_t) &((PDMA)->DSCT0_SA) + 16 * (channel_num),
					(size_t)buffers[next_buff_indx]);
		}
	}
	else
	{
		if (BUFF_IDLE != *next_buff_status)
		{
			CRITICAL_ERROR("buffer not ready\n");
		}
		runtime_hndl->buff_status[curr_buff_indx] = BUFF_READY;
		*next_buff_status = BUFF_IN_PROCCESS;
		writeRegU32((uint32_t) &((PDMA)->DSCT0_DA) + 16 * (channel_num),
				(size_t)buffers[next_buff_indx]);
	}
	runtime_hndl->curr_buff_indx = next_buff_indx;
}


void PDMA_IRQHandler(void)
{
    uint32_t done_status ;
    uint32_t status;
    uint8_t i;

    status = PDMA_GET_INT_STATUS();
    if(status & PDMA_INTSTS_ABTIF_Msk)
    {  /* abort */
        if(PDMA_GET_ABORT_STS() & PDMA_INTSTS_ABTIF_Msk)
        {
            //g_u32IsTestOver = 2;
        }
        PDMA_CLR_ABORT_FLAG(PDMA_INTSTS_ABTIF_Msk);
    }
    else if(status & PDMA_INTSTS_TDIF_Msk)
    {  /* done */
    	done_status = PDMA_GET_TD_STS();

    	for (i=0; i < MAX_NUMBER_OF_CHANNELS ;i++)
    	{
    		pdev_descriptor_t ch_pdev;
    		dma_i94xxx_instance_t *cfg_hndl;
    		dma_i94xxx_runtime_instance_t *runtime_hndl;
    		pdev_descriptor_t   callback_dev;

    		if (0 == (done_status & 0x1))
    		{
    			done_status = done_status >> 1;
    			continue;
    		}
			done_status = done_status >> 1;

			ch_pdev = channel_pdev[i];
    		if (NULL == ch_pdev)
    		{
    	    	CRITICAL_ERROR("not initialized channel\n");
    	    	continue;
    		}
    		cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(ch_pdev);
    		runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(ch_pdev);
    		callback_dev = cfg_hndl->callback_dev;
    		if ((DMA_FROM_PERIPHERAL == runtime_hndl->dma_peripheral_direction)
    				&& (NULL != callback_dev) )
    		{
    			DEV_CALLBACK_0_PARAMS(callback_dev, CALLBACK_NEW_DATA_ARRIVED);
    		}
    		update_buffer(cfg_hndl, runtime_hndl);
    	}

        PDMA_CLR_TD_FLAG(0xff);
    }
    else
    {
    	CRITICAL_ERROR("unknown interrupt !!\n");
    }
}


static uint8_t set_peripheral_dma(dma_i94xxx_instance_t *cfg_hndl,
		dma_i94xxx_runtime_instance_t *runtime_hndl)
{
	static uint32_t src_ctrl;
	static uint32_t dest_ctrl;
	static void *src_addr;
	static  void *dest_addr;
	uint8_t channel_num;
	uint8_t peripheral_type;
	uint8_t *buff;
	uint8_t dma_peripheral_direction;
	uint32_t buff_size;

	channel_num = cfg_hndl->channel_num;
	buff_size = cfg_hndl->buff_size;
	peripheral_type = cfg_hndl->peripheral_type;
	buff = runtime_hndl->buff[0];
	runtime_hndl->curr_buff_indx = 0;

	switch (peripheral_type)
	{
	case PDMA_SPI1_RX :
		src_addr =(void*) &SPI1->RX;
		src_ctrl = PDMA_SAR_FIX;
		dest_ctrl = PDMA_DAR_INC;
		dma_peripheral_direction = DMA_FROM_PERIPHERAL;
		break;

	case PDMA_SPI1_TX :
		src_addr = buff;
		src_ctrl = PDMA_SAR_INC;
		dest_addr = (void*)&SPI1->TX;
		dest_ctrl = PDMA_DAR_FIX;
		dma_peripheral_direction = DMA_TO_PERIPHERAL;
		break;

	case PDMA_SPI2_RX :
		src_addr =(void*) &SPI2->RX;
		src_ctrl = PDMA_SAR_FIX;
		dest_ctrl = PDMA_DAR_INC;
		dma_peripheral_direction = DMA_FROM_PERIPHERAL;
		break;

	case PDMA_SPI2_TX :
		src_addr = buff;
		src_ctrl = PDMA_SAR_INC;
		dest_addr = (void*)&SPI2->TX;
		dest_ctrl = PDMA_DAR_FIX;
		dma_peripheral_direction = DMA_TO_PERIPHERAL;
		break;

	default :
		return 1;
	}

    if (DMA_FROM_PERIPHERAL == dma_peripheral_direction)
    {
    	uint8_t *buff;
		buff = (uint8_t*)malloc(buff_size);
		dest_addr = buff;
		runtime_hndl->buff[0] = buff;
		buff = (uint8_t*)malloc(buff_size);
		runtime_hndl->buff[1] = buff;
    }

	/* Set source/destination address and attributes */
    PDMA_SetTransferAddr(channel_num,
    		(uint32_t)src_addr, src_ctrl, (uint32_t)dest_addr, dest_ctrl);
    /* Set request source; set basic mode. */
    PDMA_SetTransferMode(channel_num, peripheral_type, FALSE, 0);

    /* Single request type */
    PDMA_SetBurstType(channel_num, PDMA_REQ_SINGLE, PDMA_BURST_128);

    runtime_hndl->dma_peripheral_direction = dma_peripheral_direction;
    return 0;
}


/**
 * dma_i94xxx_ioctl()
 *
 * return:
 */
uint8_t dma_i94xxx_ioctl( pdev_descriptor_t apdev, const uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	uint8_t ret;
	dma_i94xxx_instance_t *cfg_hndl;
	dma_i94xxx_runtime_instance_t *runtime_hndl;
	uint8_t channel_num;
	uint8_t peripheral_type;
	uint32_t buff_size;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(apdev);

	channel_num = cfg_hndl->channel_num;
	buff_size = cfg_hndl->buff_size;
	peripheral_type = cfg_hndl->peripheral_type;

	if (MAX_NUMBER_OF_CHANNELS <= channel_num)
	{
    	CRITICAL_ERROR("channel number greater than allowed\n");
	}

	channel_pdev[channel_num] = apdev;

	ret = 0;
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
	    /* Enable PDMA clock source */
	    CLK_EnableModuleClock(PDMA_MODULE);


	    /* Enable PDMA channels */
	    PDMA_Open(1 << channel_num);

	    /* SPI master PDMA TX channel configuration */
	    /* Set transfer width (32 bits) and transfer count */
	    PDMA_SetTransferCnt(channel_num, PDMA_WIDTH_8, buff_size);

	    if (PDMA_MEM != peripheral_type)
	    {
	    	ret = set_peripheral_dma(cfg_hndl, runtime_hndl);
	    }
	    else
	    {
	    	ret = 1;/* not supported yet*/
	    }


		runtime_hndl->buff_status[0] = BUFF_IDLE ;
		runtime_hndl->buff_status[1] = BUFF_IDLE ;

	    if (0 == ret)
	    {
	    	irq_register_interrupt(PDMA_IRQn, PDMA_IRQHandler);
			irq_set_priority(PDMA_IRQn,
					OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
			irq_enable_interrupt(PDMA_IRQn);
		    PDMA_EnableInt(channel_num, PDMA_INT_TRANS_DONE);
	    }
		break;

	case DMA_I94XXX_IOCTL_GET_RX_BUFF :
		{
			uint8_t prev_buff_indx;

			/*
			 * until we not change buff state from IDLE to READY in
			 * interrupt , curr_buff_indx will not be changed
			 */
			prev_buff_indx = 1 - runtime_hndl->curr_buff_indx ;
			if (BUFF_READY == runtime_hndl->buff_status[prev_buff_indx])
			{
				*((uint8_t**)aIoctl_param1) =
						runtime_hndl->buff[prev_buff_indx];
				runtime_hndl->buff_status[prev_buff_indx] = BUFF_IDLE;
			}
		}
		break;

	case DMA_I94XXX_IOCTL_SET_TX_BUFF :
		{
			uint8_t next_curr_buff_indx;

			/*
			 * until we not change buff state from READY to IDLE in
			 * interrupt , curr_buff_indx will not be changed
			 */
			next_curr_buff_indx = 1 - runtime_hndl->curr_buff_indx ;
			if (BUFF_IDLE == runtime_hndl->buff_status[next_curr_buff_indx])
			{
				runtime_hndl->buff[next_curr_buff_indx] =
						(uint8_t*)aIoctl_param1 ;
				runtime_hndl->buff_status[next_curr_buff_indx] = BUFF_READY;
			}
		}
		break;

	default :
		return 1;
	}

	return ret;
}
