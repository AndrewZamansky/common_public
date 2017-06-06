/*
 *
 * dma_i94xxx.c
 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "dma_i94xxx.h"


#include "ISD94XXXSeries.h"

#include "irq_api.h"
#include "os_wrapper.h"

#include "_dma_i94xxx_prerequirements_check.h"

/*following line add module to available module list for dynamic device tree*/
#include "dma_i94xxx_add_component.h"


/********  defines *********************/

#define DMA_TO_PERIPHERAL	0
#define DMA_FROM_PERIPHERAL	1

#define writeRegU32(addr,val)	( (*(volatile uint32_t *)(addr)) = (val) )
#define readRegU32(addr)		 (*(volatile uint32_t *)(addr))

/********  types  *********************/

typedef enum
{
	DMA_I94XXX_BUFF_IDLE,
	DMA_I94XXX_BUFF_DMA_IN_PROCCESS,
	DMA_I94XXX_BUFF_TX_DATA_IS_FILLING,
	DMA_I94XXX_BUFF_TX_DATA_IS_READY,
	DMA_I94XXX_BUFF_RX_RADA_READY,
	DMA_I94XXX_BUFF_RX_RADA_PROCESSING
} DMA_I94XXX_buff_state_t;

/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/
#define MAX_NUMBER_OF_CHANNELS	16
struct dev_desc_t * channel_pdev[MAX_NUMBER_OF_CHANNELS] = {0};

#define DMA_I94XXX_DEBUG

#ifdef DMA_I94XXX_DEBUG
	static volatile int cnt1 = 0;
	static volatile int cnt2 = 0;
	static volatile int bytecount0 = 0;
	static volatile int bytecount1 = 0;
#endif

static void update_buffer(struct dma_i94xxx_cfg_t *cfg_hndl,
		struct dma_i94xxx_runtime_t *runtime_hndl)
{
	uint8_t	curr_dma_buff_indx;
	uint8_t	next_dma_buff_indx;
	uint8_t *next_buff_status;
	uint8_t *curr_buff_status;
	uint8_t need_external_restart;
	uint8_t channel_num;
	uint8_t **buffers;
	uint32_t reg_addr;
	uint32_t reg_val;
	struct dev_desc_t *callback_dev;

	channel_num = cfg_hndl->channel_num;
	curr_dma_buff_indx = runtime_hndl->curr_dma_buff_indx;
	next_dma_buff_indx = (curr_dma_buff_indx + 1) % NUM_OF_BUFFERS;
	curr_buff_status = &runtime_hndl->buff_status[curr_dma_buff_indx];
	next_buff_status = &runtime_hndl->buff_status[next_dma_buff_indx];
	buffers = runtime_hndl->buff;

	if (DMA_TO_PERIPHERAL == runtime_hndl->dma_peripheral_direction)
	{
		#ifdef DMA_I94XXX_DEBUG
			cnt1++;
		#endif

		*curr_buff_status = DMA_I94XXX_BUFF_IDLE;
		if (DMA_I94XXX_BUFF_TX_DATA_IS_READY == *next_buff_status)
		{
	    	need_external_restart = 0;
			*next_buff_status = DMA_I94XXX_BUFF_DMA_IN_PROCCESS;
			PDMA_SET_SRC_ADDR(channel_num, (size_t)buffers[next_dma_buff_indx]);
			runtime_hndl->curr_dma_buff_indx = next_dma_buff_indx;
		}
		else
		{
	    	need_external_restart = 1;
			callback_dev = cfg_hndl->callback_dev;
			if (NULL != callback_dev)
			{
				DEV_CALLBACK_0_PARAMS(callback_dev, CALLBACK_BUFFER_UNDERFLOW);
				if (cnt1 > 1)
				{
				#ifdef DMA_I94XXX_DEBUG
					bytecount0 = (PDMA->DSCT0_CTL >> PDMA_DSCT0_CTL_TXCNT_Pos);
					bytecount1 = (PDMA->DSCT2_CTL >> PDMA_DSCT2_CTL_TXCNT_Pos);
				#endif

					//CRITICAL_ERROR("next tx buffer not ready\n");
				}
			}
		}
    	runtime_hndl->need_external_restart = need_external_restart;
	}
	else
	{
		#ifdef DMA_I94XXX_DEBUG
			cnt2++;
		#endif

	    need_external_restart = 0;
		*curr_buff_status =	DMA_I94XXX_BUFF_RX_RADA_READY;
		if (DMA_I94XXX_BUFF_IDLE != *next_buff_status)
		{
			//CRITICAL_ERROR("next rx buffer not ready\n");//removed for debugging purposes
		}
		*next_buff_status = DMA_I94XXX_BUFF_DMA_IN_PROCCESS;
		PDMA_SET_DST_ADDR(channel_num, (size_t)buffers[next_dma_buff_indx]);
		runtime_hndl->curr_dma_buff_indx = next_dma_buff_indx;
	}

	if (1 == need_external_restart)
	{
		return;
	}
	/* reinit PDMA */
	PDMA_SET_TRANS_CNT(channel_num, runtime_hndl->buff_size_in_transfer_words);
	reg_addr = (uint32_t) &((PDMA)->DSCT0_CTL) + 16 * (channel_num);
	reg_val = readRegU32(reg_addr);
	reg_val |= 1;/* set BASIC MODE*/
	writeRegU32(reg_addr, reg_val);
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
    		struct dev_desc_t * ch_pdev;
    		struct dma_i94xxx_cfg_t *cfg_hndl;
    		struct dma_i94xxx_runtime_t *runtime_hndl;
    		struct dev_desc_t *   callback_dev;

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


static uint8_t set_peripheral_dma(struct dma_i94xxx_cfg_t *cfg_hndl,
		struct dma_i94xxx_runtime_t *runtime_hndl)
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
	uint8_t i;

	channel_num = cfg_hndl->channel_num;
	buff_size = cfg_hndl->buff_size;
	peripheral_type = cfg_hndl->peripheral_type;
	buff = runtime_hndl->buff[0];
	runtime_hndl->curr_dma_buff_indx = 0;

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

	case PDMA_I2S_RX :
		src_addr =(void*) &I2S->RXFIFO;
		src_ctrl = PDMA_SAR_FIX;
		dest_ctrl = PDMA_DAR_INC;
		dma_peripheral_direction = DMA_FROM_PERIPHERAL;
		break;

	case PDMA_I2S_TX :
		src_addr = buff;
		src_ctrl = PDMA_SAR_INC;
		dest_addr = (void*)&I2S->TXFIFO;
		dest_ctrl = PDMA_DAR_FIX;
		dma_peripheral_direction = DMA_TO_PERIPHERAL;
		break;

	case PDMA_DPWM :
		src_addr = buff;
		src_ctrl = PDMA_SAR_INC;
		dest_addr = (void*)&DPWM->DATA;
		dest_ctrl = PDMA_DAR_FIX;
		dma_peripheral_direction = DMA_TO_PERIPHERAL;
		break;

	default :
		return 1;
	}

	for (i = 0; i < NUM_OF_BUFFERS; i++)
	{
		runtime_hndl->buff[i] = (uint8_t*)malloc(buff_size);
		runtime_hndl->buff_status[i] = DMA_I94XXX_BUFF_IDLE ;
	}

    if (DMA_FROM_PERIPHERAL == dma_peripheral_direction)
    {
    	dest_addr = runtime_hndl->buff[0];
    }
    else
    {
    	runtime_hndl->need_external_restart = 1;
    }

	/* Set source/destination address and attributes */
    PDMA_SetTransferAddr(channel_num,
    		(uint32_t)src_addr, src_ctrl, (uint32_t)dest_addr, dest_ctrl);
    /* Set request source; set basic mode. */
    /* Single request type */
    PDMA_SetBurstType(channel_num, PDMA_REQ_SINGLE, PDMA_BURST_128);

    if (DMA_FROM_PERIPHERAL == dma_peripheral_direction)
    {/*setting mode will enable pdma channel*/
    	PDMA_SetTransferMode(channel_num, peripheral_type, FALSE, 0);
    }


    runtime_hndl->dma_peripheral_direction = dma_peripheral_direction;
    return 0;
}


static void set_transfer_size(struct dma_i94xxx_runtime_t *runtime_hndl,
		uint8_t channel_num, uint32_t transfer_word_size, uint32_t buff_size)
{
	uint32_t   buff_size_in_transfer_words;

	switch (transfer_word_size)
    {
    case PDMA_WIDTH_8:
    	buff_size_in_transfer_words = buff_size;
    	break;
    case PDMA_WIDTH_16:
    	buff_size_in_transfer_words = buff_size / 2;
    	break;
    case PDMA_WIDTH_32:
    	buff_size_in_transfer_words = buff_size / 4;
    	break;
    default:
    	CRITICAL_ERROR("wrong transfer_word_size\n");
    	break;
    }
    runtime_hndl->buff_size_in_transfer_words = buff_size_in_transfer_words;
    /* SPI master PDMA TX channel configuration */
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(channel_num,
    		transfer_word_size, buff_size_in_transfer_words);

}


static uint8_t start_dma_i94xxx_device(struct dev_desc_t *adev,
		struct dma_i94xxx_cfg_t *cfg_hndl,
		struct dma_i94xxx_runtime_t *runtime_hndl)
{
	uint8_t channel_num;
	uint8_t peripheral_type;
	uint8_t ret;
	uint32_t transfer_word_size;
	uint32_t buff_size;

	ret = 0;
	channel_num = cfg_hndl->channel_num;
	peripheral_type = cfg_hndl->peripheral_type;
	buff_size = cfg_hndl->buff_size;
	transfer_word_size = cfg_hndl->transfer_word_size;

	if (MAX_NUMBER_OF_CHANNELS <= channel_num)
	{
    	CRITICAL_ERROR("channel number greater than allowed\n");
	}

	channel_pdev[channel_num] = adev;

    /* Enable PDMA clock source */
    CLK_EnableModuleClock(PDMA_MODULE);


    /* Enable PDMA channels */
    PDMA_Open(1 << channel_num);


    if (PDMA_MEM != peripheral_type)
    {
    	ret = set_peripheral_dma(cfg_hndl, runtime_hndl);
    }
    else
    {
    	CRITICAL_ERROR("not supported yet\n");
    	return 1;
    }


    set_transfer_size(runtime_hndl,
    		channel_num, transfer_word_size, buff_size);

    if (0 == ret)
    {
    	irq_register_interrupt(PDMA_IRQn, PDMA_IRQHandler);
		irq_set_priority(PDMA_IRQn,
				OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
		irq_enable_interrupt(PDMA_IRQn);
	    PDMA_EnableInt(channel_num, PDMA_INT_TRANS_DONE);
    }
    return ret;
}


static uint8_t get_full_rx_buffer(struct dma_i94xxx_cfg_t *cfg_hndl,
		struct dma_i94xxx_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *buff_size)
{
	uint8_t next_supplied_rx_buffer;
	uint8_t *buffer_state;

	if (DMA_FROM_PERIPHERAL != runtime_hndl->dma_peripheral_direction)
	{
		CRITICAL_ERROR("rx buffer could not be set to this type of dma \n");
		return 1;
	}

	/*
	 * until we not change buff state from IDLE to READY in
	 * interrupt , curr_dma_buff_indx will not be changed
	 */
	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer ;
	buffer_state = &runtime_hndl->buff_status[next_supplied_rx_buffer];
	if (DMA_I94XXX_BUFF_RX_RADA_READY == *buffer_state)
	{
		*buff = runtime_hndl->buff[next_supplied_rx_buffer];
		*buff_size = cfg_hndl->buff_size;
		*buffer_state =	DMA_I94XXX_BUFF_RX_RADA_PROCESSING;
	}
	else
	{
		*buff = NULL;
		*buff_size = 0;
	}
	return 0;
}


static uint8_t release_rx_buffer(struct dma_i94xxx_cfg_t *cfg_hndl,
		struct dma_i94xxx_runtime_t *runtime_hndl)
{
	uint8_t next_supplied_rx_buffer;

	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer;
	runtime_hndl->buff_status[next_supplied_rx_buffer] = DMA_I94XXX_BUFF_IDLE;
	next_supplied_rx_buffer = (next_supplied_rx_buffer + 1) % NUM_OF_BUFFERS;
	runtime_hndl->next_supplied_rx_buffer = next_supplied_rx_buffer;
	return 0;
}


static uint8_t get_empty_tx_buffer(struct dma_i94xxx_cfg_t *cfg_hndl,
		struct dma_i94xxx_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *buff_size)
{
	uint8_t next_supplied_tx_buffer;
	uint8_t *buffer_state;
	struct dev_desc_t *callback_dev;

	if (DMA_TO_PERIPHERAL != runtime_hndl->dma_peripheral_direction)
	{
		CRITICAL_ERROR("tx buffer could not be set to this type of dma \n");
		return 1;
	}

	/*
	 * until we not change buff state from IDLE to READY in
	 * interrupt , curr_dma_buff_indx will not be changed
	 */
	next_supplied_tx_buffer = runtime_hndl->next_supplied_tx_buffer ;
	buffer_state = &runtime_hndl->buff_status[next_supplied_tx_buffer];
	if (DMA_I94XXX_BUFF_IDLE == *buffer_state)
	{
		*buff = runtime_hndl->buff[next_supplied_tx_buffer];
		*buff_size = cfg_hndl->buff_size;
		*buffer_state = DMA_I94XXX_BUFF_TX_DATA_IS_FILLING;
	}
	else
	{
		callback_dev = cfg_hndl->callback_dev;
		#ifdef DMA_I94XXX_DEBUG
			bytecount0 = (PDMA->DSCT0_CTL >> PDMA_DSCT0_CTL_TXCNT_Pos);
			bytecount1 = (PDMA->DSCT2_CTL >> PDMA_DSCT2_CTL_TXCNT_Pos);
		#endif
		if (NULL != callback_dev)
		{
			DEV_CALLBACK_0_PARAMS(callback_dev, CALLBACK_BUFFER_OVERFLOW);
		}
		//CRITICAL_ERROR("next tx buffer is still busy \n");
		*buff = NULL;
		*buff_size = 0;
		return 1;
	}
	return 0;
}


static uint8_t release_tx_buffer(struct dma_i94xxx_cfg_t *cfg_hndl,
		struct dma_i94xxx_runtime_t *runtime_hndl)
{
	uint8_t	next_supplied_tx_buffer;
	uint8_t *supplied_tx_buffer_status;
	uint8_t peripheral_type;
	uint8_t channel_num;
	uint8_t	next_dma_buff_indx;
	uint8_t	need_external_restart;
	uint8_t **buffers;

	if (DMA_TO_PERIPHERAL != runtime_hndl->dma_peripheral_direction)
	{
		CRITICAL_ERROR("tx buffer could not be set to this type of dma \n");
		return 1;
	}

	next_supplied_tx_buffer = runtime_hndl->next_supplied_tx_buffer;
	supplied_tx_buffer_status =
			&runtime_hndl->buff_status[next_supplied_tx_buffer];

	if (DMA_I94XXX_BUFF_TX_DATA_IS_FILLING == *supplied_tx_buffer_status)
	{
		*supplied_tx_buffer_status = DMA_I94XXX_BUFF_TX_DATA_IS_READY;
		runtime_hndl->next_supplied_tx_buffer =
				(next_supplied_tx_buffer + 1) % NUM_OF_BUFFERS;

		need_external_restart = runtime_hndl->need_external_restart;
		if ( 2 == need_external_restart )
		{
			next_dma_buff_indx = (next_supplied_tx_buffer - 1) % NUM_OF_BUFFERS;
			runtime_hndl->buff_status[next_dma_buff_indx] =
									DMA_I94XXX_BUFF_DMA_IN_PROCCESS;
			runtime_hndl->curr_dma_buff_indx = next_dma_buff_indx;
			peripheral_type = cfg_hndl->peripheral_type;
			channel_num = cfg_hndl->channel_num;
			buffers = runtime_hndl->buff;
			PDMA_SET_TRANS_CNT(channel_num,
					runtime_hndl->buff_size_in_transfer_words);
			PDMA_SET_SRC_ADDR(channel_num, (size_t)buffers[next_dma_buff_indx]);
			PDMA_SetTransferMode( channel_num, peripheral_type, FALSE, 0);
			need_external_restart = 0;
		}
		else if ( 1 == need_external_restart )
		{
			need_external_restart++;
		}
		runtime_hndl->need_external_restart = need_external_restart;
	}
	return 0;

}


/**
 * dma_i94xxx_ioctl()
 *
 * return:
 */
uint8_t dma_i94xxx_ioctl( struct dev_desc_t *adev, const uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	uint8_t ret;
	struct dma_i94xxx_cfg_t *cfg_hndl;
	struct dma_i94xxx_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);


	ret = 0;
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		ret = start_dma_i94xxx_device(adev, cfg_hndl, runtime_hndl);
		break;

	case DMA_I94XXX_IOCTL_GET_RX_BUFF :
		ret = get_full_rx_buffer(cfg_hndl,
				runtime_hndl, aIoctl_param1, aIoctl_param2);
		break;

	case DMA_I94XXX_IOCTL_RELEASE_RX_BUFF :
		ret = release_rx_buffer(cfg_hndl, runtime_hndl);
		break;

	case DMA_I94XXX_IOCTL_GET_EMPTY_TX_BUFF :
		ret = get_empty_tx_buffer(cfg_hndl,
				runtime_hndl, aIoctl_param1, aIoctl_param2);
		break;

	case DMA_I94XXX_IOCTL_RELEASE_TX_BUFF :
		ret = release_tx_buffer(cfg_hndl, runtime_hndl);
		break;

	default :
		return 1;
	}

	return ret;
}
