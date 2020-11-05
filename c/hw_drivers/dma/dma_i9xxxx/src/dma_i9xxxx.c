/*
 *
 * dma_i9xxxx.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "errors_api.h"

#include "dev_management_api.h"

#include "dma_i9xxxx.h"
#include "dma_i9xxxx_api.h"

#ifdef CONFIG_I94XXX
	#include "I94100.h"
	#include "clock_control_i94xxx_api.h"
#elif CONFIG_I96XXX_M0
	#include "I96100.h"
	#include "clock_control_i96xxx_m0_api.h"
#endif

#include "irq_api.h"
#include "os_wrapper.h"


/*following line add module to available module list for dynamic device tree*/


#if !defined(INTERRUPT_PRIORITY_FOR_DMA)
	#error "INTERRUPT_PRIORITY_FOR_DMA should be defined"
#endif

#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_DMA)
	#error "priority should be lower then maximal priority for os syscalls"
#endif

#ifdef CONFIG_I96XXX_M0
	#define PDMA_INTSTS_ALIGNF_Msk    PDMA_INT0STS_ALIGNF_Msk
	#define PDMA_INTSTS_ABTIF_Msk     PDMA_INT0STS_ABTIF_Msk
	#define PDMA_INTSTS_TDIF_Msk      PDMA_INT0STS_TDIF_Msk
	#define SPI_I2SCTL_TXEN_Msk       SPI2_I2SCTL_TXEN_Msk
	#define SPI_PDMACTL_TXPDMAEN_Msk  SPI2_PDMACTL_TXPDMAEN_Msk
#endif

#define DMA_TO_PERIPHERAL   0
#define DMA_FROM_PERIPHERAL 1
#define UNKNOWN_DMA_TYPE    0xff

#define writeRegU32(addr,val)   ( (*(volatile uint32_t *)(addr)) = (val) )
#define readRegU32(addr)        (*(volatile uint32_t *)(addr))

#define PDMA_GET_TRANS_CNT(u32Ch)                               \
	((PDMA->DSCT[(u32Ch)].CTL & PDMA_DSCT_CTL_TXCNT_Msk) >>     \
												PDMA_DSCT_CTL_TXCNT_Pos)

enum DMA_I9XXXX_buff_state_e {
	DMA_I9XXXX_BUFF_IDLE,
	DMA_I9XXXX_BUFF_DMA_IN_PROCCESS,
	DMA_I9XXXX_BUFF_TX_DATA_IS_FILLING,
	DMA_I9XXXX_BUFF_TX_DATA_IS_READY,
	DMA_I9XXXX_BUFF_RX_RADA_READY,
	DMA_I9XXXX_BUFF_RX_RADA_PROCESSING
};

#define MAX_NUMBER_OF_CHANNELS	16
static struct dev_desc_t * channel_pdev[MAX_NUMBER_OF_CHANNELS] = {0};

//#define DMA_I9XXXX_DEBUG

#ifdef DMA_I9XXXX_DEBUG
	volatile int dma_i9xxxx_dbg_cnt1 = 0;
	volatile int dma_i9xxxx_dbg_cnt2 = 0;
	volatile int dma_i9xxxx_dbg_cnt3 = 0;
	volatile int dma_i9xxxx_dbg_cnt4 = 0;
	volatile int dma_i9xxxx_dbg_cnt5 = 0;
	volatile int dma_i9xxxx_dbg_cnt6 = 0;
	volatile int dma_i9xxxx_dbg_cnt7 = 0;
	volatile int dma_i9xxxx_dbg_cnt8 = 0;
	volatile  struct dma_i9xxxx_runtime_t *runtime_hndl_tmp;

	volatile int g_status = 0;
	volatile int bytecount0 = 0;
	volatile int bytecount1 = 0;
	volatile int bytecount2 = 0;
#endif




/* func : put_new_buffer_in_pdma
 *
 */
static void put_new_buffer_in_pdma(uint8_t channel_num, uint8_t *buff_status,
		struct dma_i9xxxx_runtime_t *runtime_hndl, uint8_t next_dma_buff_indx)
{
	DSCT_T *DSCT;
	uint32_t reg_addr;
	uint32_t reg_val;

	if (1 == runtime_hndl->needed_full_dma_start)
	{
		return;
	}

	/* reinit PDMA */
	buff_status[next_dma_buff_indx] = DMA_I9XXXX_BUFF_DMA_IN_PROCCESS;
	runtime_hndl->curr_dma_buff_indx = next_dma_buff_indx;

	PDMA_SET_TRANS_CNT(channel_num, runtime_hndl->buff_size_in_transfer_words);
	DSCT = &((PDMA)->DSCT[0]);
	reg_addr = (uint32_t) &(DSCT->CTL) + 16 * (channel_num);
	reg_val = readRegU32(reg_addr);
	reg_val |= 1;/* set BASIC MODE*/
	writeRegU32(reg_addr, reg_val);
}


/* func : update_tx_buffer
 *
 */
static void update_tx_buffer(struct dev_desc_t * ch_pdev,
		struct dma_i9xxxx_runtime_t *runtime_hndl)
{
	uint8_t	curr_dma_buff_indx;
	uint8_t	next_dma_buff_indx;
	uint8_t look_foward_tx_buffer;
	struct dev_desc_t *callback_dev;
	struct dma_i9xxxx_cfg_t *cfg_hndl;
	uint8_t *buff_status;
	uint8_t channel_num;
	uint8_t **buffers;
	uint8_t num_of_buffers;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(dma_i9xxxx, ch_pdev);
	buff_status = runtime_hndl->buff_status;
	callback_dev = cfg_hndl->callback_dev;
	curr_dma_buff_indx = runtime_hndl->curr_dma_buff_indx;
	num_of_buffers = cfg_hndl->num_of_buffers;
	next_dma_buff_indx = (curr_dma_buff_indx + 1) % num_of_buffers;
	look_foward_tx_buffer = (next_dma_buff_indx + 1) % num_of_buffers;

	#ifdef DMA_I9XXXX_DEBUG
	dma_i9xxxx_dbg_cnt1++;
	#endif

	if (DMA_I9XXXX_BUFF_TX_DATA_IS_READY !=
			runtime_hndl->buff_status[look_foward_tx_buffer])
	{
		if (NULL != callback_dev)
		{
			DEV_CALLBACK_1_PARAMS( callback_dev,
					CALLBACK_TX_BUFFER_UNDERFLOW_THRESHOLD_REACHED, ch_pdev);
		}
		#ifdef DMA_I9XXXX_DEBUG
		dma_i9xxxx_dbg_cnt7++;
		bytecount1 = PDMA_GET_TRANS_CNT(2);
		#endif
	}


	buff_status[curr_dma_buff_indx] = DMA_I9XXXX_BUFF_IDLE;
	if (DMA_I9XXXX_BUFF_TX_DATA_IS_READY == buff_status[next_dma_buff_indx])
	{
		channel_num = cfg_hndl->channel_num;
		buffers = runtime_hndl->buff;
		PDMA_SET_SRC_ADDR(channel_num, (size_t)buffers[next_dma_buff_indx]);
		put_new_buffer_in_pdma(cfg_hndl->channel_num,
				buff_status, runtime_hndl, next_dma_buff_indx);
		return;
	}

	if (NULL != callback_dev)
	{
		DEV_CALLBACK_1_PARAMS(
				callback_dev, CALLBACK_TX_BUFFER_UNDERFLOW, ch_pdev);
	}
	runtime_hndl->needed_full_dma_start = 1;
}


/* func : update_rx_buffer
 *
 */
static void update_rx_buffer(struct dev_desc_t * ch_pdev,
		struct dma_i9xxxx_runtime_t *runtime_hndl)
{
	uint8_t	curr_dma_buff_indx;
	uint8_t	next_dma_buff_indx;
	struct dev_desc_t *callback_dev;
	struct dma_i9xxxx_cfg_t *cfg_hndl;
	uint8_t *buff_status;
	uint8_t **buffers;
	uint8_t channel_num;
	uint8_t num_of_buffers;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(dma_i9xxxx, ch_pdev);
	buff_status = runtime_hndl->buff_status;
	callback_dev = cfg_hndl->callback_dev;
	curr_dma_buff_indx = runtime_hndl->curr_dma_buff_indx;
	num_of_buffers = cfg_hndl->num_of_buffers;
	next_dma_buff_indx = (curr_dma_buff_indx + 1) % num_of_buffers;

	#ifdef DMA_I9XXXX_DEBUG
		dma_i9xxxx_dbg_cnt2++;
	#endif

	if (NULL != callback_dev)
	{
		DEV_CALLBACK_1_PARAMS(callback_dev, CALLBACK_NEW_DATA_ARRIVED, ch_pdev);
	}

	buff_status[curr_dma_buff_indx] =	DMA_I9XXXX_BUFF_RX_RADA_READY;
	if (DMA_I9XXXX_BUFF_IDLE != buff_status[next_dma_buff_indx])
	{
		//CRITICAL_ERROR("next rx buffer not ready\n");
		next_dma_buff_indx = curr_dma_buff_indx;
		if (NULL != callback_dev)
		{
			DEV_CALLBACK_1_PARAMS(
					callback_dev, CALLBACK_RX_BUFFER_OVERFLOW, ch_pdev);
		}

	}

	channel_num = cfg_hndl->channel_num;
	buffers = runtime_hndl->buff;
	PDMA_SET_DST_ADDR(channel_num, (size_t)buffers[next_dma_buff_indx]);
	put_new_buffer_in_pdma(channel_num,
			buff_status, runtime_hndl, next_dma_buff_indx);
}


/* func : transmit_done
 *
 */
static void transmit_done(void)
{
	uint8_t i;
	uint32_t curr_mask;
	uint32_t done_status;
	struct dev_desc_t * ch_pdev;
	struct dma_i9xxxx_runtime_t *runtime_hndl;

	done_status = PDMA_GET_TD_STS();
	curr_mask = 1;
	for (i = 0; i < MAX_NUMBER_OF_CHANNELS; i++)
	{
		if (0 == (done_status & curr_mask))
		{
			curr_mask = curr_mask << 1;
			continue;
		}

		ch_pdev = channel_pdev[i];
		if (NULL == ch_pdev)
		{
			CRITICAL_ERROR("not initialized channel\n");
			continue;
		}
		runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(dma_i9xxxx, ch_pdev);
		if (DMA_TO_PERIPHERAL == runtime_hndl->dma_peripheral_direction)
		{
			update_tx_buffer(ch_pdev, runtime_hndl);
		}
		else
		{
			update_rx_buffer(ch_pdev, runtime_hndl);
		}
		PDMA->TDSTS = curr_mask;
		curr_mask = curr_mask << 1;
	}
}


/* func : PDMA_IRQHandler
 *
 */
static void PDMA_IRQHandler(void)
{
	uint32_t status;

	status = PDMA_GET_INT_STATUS();
	if(status & PDMA_INTSTS_ALIGNF_Msk)
	{
		#ifdef DMA_I9XXXX_DEBUG
			dma_i9xxxx_dbg_cnt3++;
		#endif
		//CRITICAL_ERROR("dma align trap !!\n");
	}
	status &= ~(PDMA_INTSTS_ALIGNF_Msk);

	if(status & PDMA_INTSTS_ABTIF_Msk)
	{
		CRITICAL_ERROR("dma abort trap !!\n");
		if(PDMA_GET_ABORT_STS() & PDMA_INTSTS_ABTIF_Msk)
		{
			//g_u32IsTestOver = 2;
		}
		PDMA_CLR_ABORT_FLAG(PDMA_INTSTS_ABTIF_Msk);
	}
	status &= ~(PDMA_INTSTS_ABTIF_Msk);

	if(status & PDMA_INTSTS_TDIF_Msk)
	{
		transmit_done();
	}
	status &= ~(PDMA_INTSTS_TDIF_Msk);

	if(status)
	{
		CRITICAL_ERROR("unknown interrupt !!\n");
	}
}


static uint8_t populate_dma_addresses_and_ctl(uint8_t peripheral_type,
		uint32_t *src_addr, uint32_t *dest_addr,
		uint32_t *src_ctrl, uint32_t *dest_ctrl)
{
	switch (peripheral_type)
	{
#ifdef CONFIG_I94XXX
	case PDMA_SPI1_RX :
		*src_addr =(uint32_t) &SPI1->RX;
		*src_ctrl = PDMA_SAR_FIX;
		*dest_ctrl = PDMA_DAR_INC;
		return DMA_FROM_PERIPHERAL;
	case PDMA_SPI1_TX :
		*dest_addr = (uint32_t)&SPI1->TX;
		*src_ctrl = PDMA_SAR_INC;
		*dest_ctrl = PDMA_DAR_FIX;
		return DMA_TO_PERIPHERAL;
#endif
	case PDMA_SPI2_RX :
		*src_addr = (uint32_t) &SPI2->RX;
		*src_ctrl = PDMA_SAR_FIX;
		*dest_ctrl = PDMA_DAR_INC;
		return DMA_FROM_PERIPHERAL;
	case PDMA_SPI2_TX :
		*src_ctrl = PDMA_SAR_INC;
		*dest_addr = (uint32_t) &SPI2->TX;
		*dest_ctrl = PDMA_DAR_FIX;
		return DMA_TO_PERIPHERAL;
	case PDMA_I2S0_RX :
		*src_addr = (uint32_t) &I2S0->RXFIFO;
		*src_ctrl = PDMA_SAR_FIX;
		*dest_ctrl = PDMA_DAR_INC;
		return DMA_FROM_PERIPHERAL;
	case PDMA_I2S0_TX :
		*src_ctrl = PDMA_SAR_INC;
		*dest_addr = (uint32_t)&I2S0->TXFIFO;
		*dest_ctrl = PDMA_DAR_FIX;
		return DMA_TO_PERIPHERAL;
	case PDMA_DPWM :
		*src_ctrl = PDMA_SAR_INC;
		*dest_addr = (uint32_t)&DPWM->FIFO;
		*dest_ctrl = PDMA_DAR_FIX;
		return DMA_TO_PERIPHERAL;
	case PDMA_DMIC_RX :
		*src_ctrl = PDMA_SAR_FIX;
		*src_addr = (uint32_t) &DMIC->FIFO;
		*dest_ctrl = PDMA_DAR_INC;
		return DMA_FROM_PERIPHERAL;
	default :
		*src_addr = (uint32_t)NULL;
		*dest_addr = (uint32_t)NULL;
		*dest_ctrl = 0;
		*src_ctrl = 0;
		return UNKNOWN_DMA_TYPE;
	}
}


static void init_buffers(struct dma_i9xxxx_cfg_t *cfg_hndl,
		struct dma_i9xxxx_runtime_t *runtime_hndl)
{
	uint8_t i;
	uint8_t *buff;
	uint8_t num_of_buffers;
	size_t  buff_size;

	num_of_buffers = cfg_hndl->num_of_buffers;
	buff_size = cfg_hndl->buff_size;

	if(NULL == runtime_hndl->buff)
	{
		runtime_hndl->buff =
				(uint8_t**)os_safe_malloc(sizeof(uint8_t*) * num_of_buffers);
		errors_api_check_if_malloc_succeed(runtime_hndl->buff);
		runtime_hndl->buff_status = (uint8_t*)os_safe_malloc(num_of_buffers);
		errors_api_check_if_malloc_succeed(runtime_hndl->buff_status);

		for (i = 0; i < num_of_buffers; i++)
		{
			buff = (uint8_t*)os_safe_malloc(buff_size);
			errors_api_check_if_malloc_succeed(buff);
			runtime_hndl->buff[i] = buff;
		}
	}
	for (i = 0; i < num_of_buffers; i++)
	{
		runtime_hndl->buff_status[i] = DMA_I9XXXX_BUFF_IDLE ;
	}
	runtime_hndl->next_supplied_tx_buffer = 0;
}


/* func : set_peripheral_dma
 *
 */
static uint8_t set_peripheral_dma(struct dma_i9xxxx_cfg_t *cfg_hndl,
		struct dma_i9xxxx_runtime_t *runtime_hndl)
{
	uint32_t  src_ctrl;
	uint32_t  dest_ctrl;
	uint32_t  src_addr;
	uint32_t  dest_addr;
	uint8_t channel_num;
	uint8_t peripheral_type;
	uint8_t dma_peripheral_direction;

	channel_num = cfg_hndl->channel_num;
	peripheral_type = cfg_hndl->peripheral_type;
	runtime_hndl->curr_dma_buff_indx = 0;

	dma_peripheral_direction = populate_dma_addresses_and_ctl(
			peripheral_type, &src_addr, &dest_addr, &src_ctrl, &dest_ctrl);
	runtime_hndl->dma_peripheral_direction = dma_peripheral_direction;

	if (UNKNOWN_DMA_TYPE == dma_peripheral_direction)
	{
		CRITICAL_ERROR("unknown DMA type");
		return 1;
	}

	init_buffers(cfg_hndl, runtime_hndl);

	if (DMA_FROM_PERIPHERAL == dma_peripheral_direction)
	{
		dest_addr = (uint32_t)runtime_hndl->buff[0];// TODO : redesign RX
		runtime_hndl->curr_dma_buff_indx = 0;
	}
	else
	{
		runtime_hndl->needed_full_dma_start = 1;
		runtime_hndl->curr_dma_buff_indx = cfg_hndl->num_of_buffers - 1;
	}

	/* Set source/destination address and attributes */
	PDMA_SetTransferAddr(channel_num,
			src_addr, src_ctrl, dest_addr, dest_ctrl);
	/* Set request source; set basic mode. */
	/* Single request type */
	PDMA_SetBurstType(channel_num, PDMA_REQ_SINGLE, PDMA_BURST_128);

	if (DMA_FROM_PERIPHERAL == dma_peripheral_direction)
	{/*setting mode will enable pdma channel*/
		PDMA_SetTransferMode(channel_num, peripheral_type, FALSE, 0);
	}
	return 0;
}


/* func : set_transfer_size
 *
 */
static void set_transfer_size(struct dma_i9xxxx_runtime_t *runtime_hndl,
		uint8_t channel_num, uint16_t transfer_word_size, size_t buff_size)
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


/* func : start_dma_i9xxxx_device
 *
 */
static uint8_t start_dma_i9xxxx_device(struct dev_desc_t *adev,
		struct dma_i9xxxx_cfg_t *cfg_hndl,
		struct dma_i9xxxx_runtime_t *runtime_hndl)
{
	uint8_t channel_num;
	uint8_t peripheral_type;
	uint8_t ret;
	uint16_t transfer_word_size;
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

	if (0 != channel_pdev[channel_num])
	{
		CRITICAL_ERROR("channel already used\n");
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

	set_transfer_size(runtime_hndl, channel_num, transfer_word_size, buff_size);

	if (0 == ret)
	{
		irq_register_interrupt(PDMA_IRQn, PDMA_IRQHandler);
		irq_set_priority(PDMA_IRQn, INTERRUPT_PRIORITY_FOR_DMA);
		irq_enable_interrupt(PDMA_IRQn);
		PDMA_EnableInt(channel_num, PDMA_INT_TRANS_DONE);
		runtime_hndl->init_done = 1;
	}

	return ret;
}


static uint8_t get_full_rx_buffer(struct dma_i9xxxx_cfg_t *cfg_hndl,
		struct dma_i9xxxx_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *buff_size)
{
	uint8_t next_supplied_rx_buffer;
	uint8_t *buffer_state;
	struct dev_desc_t *callback_dev;

	if (0 == runtime_hndl->init_done)
	{
		*buff = NULL;
		*buff_size = 0;
		return 1;
	}

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
	if (DMA_I9XXXX_BUFF_RX_RADA_READY == *buffer_state)
	{
		*buff = runtime_hndl->buff[next_supplied_rx_buffer];
		*buff_size = cfg_hndl->buff_size;
		*buffer_state =	DMA_I9XXXX_BUFF_RX_RADA_PROCESSING;
	}
	else
	{
		callback_dev = cfg_hndl->callback_dev;
		if (NULL != callback_dev)
		{
			DEV_CALLBACK_0_PARAMS(callback_dev, CALLBACK_RX_BUFFER_UNDERFLOW);
		}
		*buff = NULL;
		*buff_size = 0;
	}
	return 0;
}


static uint8_t release_rx_buffer(struct dma_i9xxxx_cfg_t *cfg_hndl,
		struct dma_i9xxxx_runtime_t *runtime_hndl)
{
	uint8_t next_supplied_rx_buffer;
	uint8_t num_of_buffers;
	uint8_t *buffer_state;

	next_supplied_rx_buffer = runtime_hndl->next_supplied_rx_buffer;
	buffer_state = &runtime_hndl->buff_status[next_supplied_rx_buffer];
	if (DMA_I9XXXX_BUFF_RX_RADA_PROCESSING != *buffer_state)
	{
		return 1;
	}
	*buffer_state = DMA_I9XXXX_BUFF_IDLE;
	num_of_buffers = cfg_hndl->num_of_buffers;
	next_supplied_rx_buffer = (next_supplied_rx_buffer + 1) % num_of_buffers;
	runtime_hndl->next_supplied_rx_buffer = next_supplied_rx_buffer;
	return 0;
}



static uint8_t get_empty_tx_buffer(struct dev_desc_t *ch_pdev,
		struct dma_i9xxxx_cfg_t *cfg_hndl,
		struct dma_i9xxxx_runtime_t *runtime_hndl,
		uint8_t **buff, size_t *buff_size)
{
	uint8_t next_supplied_tx_buffer;
	uint8_t look_foward_tx_buffer;
	uint8_t *buffer_state;
	struct dev_desc_t *callback_dev;
	uint8_t num_of_buffers;

	if (0 == runtime_hndl->init_done)
	{
		*buff = NULL;
		*buff_size = 0;
		return 1;
	}

	if (DMA_TO_PERIPHERAL != runtime_hndl->dma_peripheral_direction)
	{
		CRITICAL_ERROR("tx buffer could not be set to this type of dma \n");
		return 1;
	}

	/*
	 * until we not change buff state from IDLE to READY in
	 * interrupt , curr_dma_buff_indx will not be changed
	 */
	#ifdef DMA_I9XXXX_DEBUG
	dma_i9xxxx_dbg_cnt4++;
	#endif
	callback_dev = cfg_hndl->callback_dev;
	num_of_buffers = cfg_hndl->num_of_buffers;

	next_supplied_tx_buffer = runtime_hndl->next_supplied_tx_buffer ;
	look_foward_tx_buffer = (next_supplied_tx_buffer + 1) % num_of_buffers;


	buffer_state = &runtime_hndl->buff_status[look_foward_tx_buffer];
	if (DMA_I9XXXX_BUFF_IDLE != *buffer_state)
	{
		#ifdef DMA_I9XXXX_DEBUG
		dma_i9xxxx_dbg_cnt5++;
		#endif
		if (NULL != callback_dev)
		{
			DEV_CALLBACK_1_PARAMS( callback_dev,
					CALLBACK_TX_BUFFER_OVERFLOW_THRESHOLD_REACHED, ch_pdev);
		}
	}


	buffer_state = &runtime_hndl->buff_status[next_supplied_tx_buffer];
	if (DMA_I9XXXX_BUFF_IDLE == *buffer_state)
	{
		*buff = runtime_hndl->buff[next_supplied_tx_buffer];
		*buff_size = cfg_hndl->buff_size;
		*buffer_state = DMA_I9XXXX_BUFF_TX_DATA_IS_FILLING;
	}
	else
	{
		#ifdef DMA_I9XXXX_DEBUG
//			bytecount0 = PDMA_GET_TRANS_CNT(0);
//			bytecount1 = PDMA_GET_TRANS_CNT(1);
//			bytecount2 = PDMA_GET_TRANS_CNT(2);
		#endif
		if (NULL != callback_dev)
		{
			DEV_CALLBACK_0_PARAMS(callback_dev, CALLBACK_TX_BUFFER_OVERFLOW);
		}
		//TODO : add overflow sequence
		// maybe 4th buffer will be needed . with second buffer as initial one.
		// in NuAudio project , maybe it will happen when audio
		// process time will drop , suddenly, significantly  .
		//CRITICAL_ERROR("next tx buffer is still busy \n");
		*buff = NULL;
		*buff_size = 0;
		return 1;
	}
	return 0;
}


static void enable_peripheral_output( uint8_t peripheral_type,
			struct dev_desc_t *peripheral_dev, uint8_t channel_num)
{
	// Need to use these variables to keep modularity and abstraction.
//	struct I2S_onSPI_i9xxxx_cfg_t *cfg_hndl;
//	SPI_T	*I2S_SPI_module;

	switch (peripheral_type)
	{
#ifdef CONFIG_I94XXX
	case PDMA_SPI1_TX :

		//TODO: Doesn't work, using direct bit access

//		cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(dma_i9xxxx, peripheral_dev);
//		I2S_SPI_module = (SPI_T *)cfg_hndl->base_address;
//		SPI_I2S_ENABLE_TX(I2S_SPI_module);
//		SPI_I2S_ENABLE_TXDMA(I2S_SPI_module);

		//Currently using I2S protocol on SPI bus.
		// TODO: Need identifier for different protocols.
		SPI1->I2SCTL &= ~SPI_I2SCTL_TXEN_Msk;
		SPI1->I2SCTL |= SPI_I2SCTL_TXEN_Msk;

		SPI1->PDMACTL &= ~SPI_PDMACTL_TXPDMAEN_Msk;
		SPI1->PDMACTL |= SPI_PDMACTL_TXPDMAEN_Msk;
		PDMA_Trigger(channel_num); // needed for SPI module

		break;
#endif
	case PDMA_SPI2_TX :

		//Currently using I2S protocol on SPI bus.
		// TODO: Need identifier for different protocols.
		SPI2->I2SCTL &= ~SPI_I2SCTL_TXEN_Msk;
		SPI2->I2SCTL |= SPI_I2SCTL_TXEN_Msk;

		SPI2->PDMACTL &= ~SPI_PDMACTL_TXPDMAEN_Msk;
		SPI2->PDMACTL |= SPI_PDMACTL_TXPDMAEN_Msk;
		PDMA_Trigger(channel_num); // needed for SPI module

		break;

	case PDMA_I2S0_TX :
		I2S0->CTL0 &= ~(I2S_CTL0_TXEN_Msk | I2S_CTL0_TXPDMAEN_Msk);
		I2S0->CTL0 |= (I2S_CTL0_TXEN_Msk | I2S_CTL0_TXPDMAEN_Msk);
		break;

	case PDMA_DPWM :
		//DEV_IOCTL_0_PARAMS(peripheral_dev , IOCTL_DEVICE_START );
		DPWM->PDMACTL |= DPWM_PDMACTL_PDMAEN_Msk;
		DPWM->CTL |= (DPWM_CTL_DPWMEN_Msk | DPWM_CTL_DRVEN_Msk);
		break;

	default :
		CRITICAL_ERROR("TODO: add additional periperals \n");
	}
}


static uint8_t release_tx_buffer(struct dma_i9xxxx_cfg_t *cfg_hndl,
		struct dma_i9xxxx_runtime_t *runtime_hndl)
{
	uint8_t	next_supplied_tx_buffer;
	uint8_t *supplied_tx_buffer_status;
	uint8_t peripheral_type;
	uint8_t channel_num;
	uint8_t	next_dma_buff_indx;
	uint8_t	prefilled_buffers;
	uint8_t **buffers;
	struct dev_desc_t *peripheral_dev;
	uint8_t num_of_buffers;
	uint8_t    num_of_prefilled_buffer_before_tx_start;

	if (DMA_TO_PERIPHERAL != runtime_hndl->dma_peripheral_direction)
	{
		CRITICAL_ERROR("tx buffer could not be set to this type of dma \n");
		return 1;
	}

	next_supplied_tx_buffer = runtime_hndl->next_supplied_tx_buffer;
	supplied_tx_buffer_status =
			&runtime_hndl->buff_status[next_supplied_tx_buffer];

	if (DMA_I9XXXX_BUFF_TX_DATA_IS_FILLING != *supplied_tx_buffer_status)
	{
		#ifdef DMA_I9XXXX_DEBUG
//			bytecount0 = PDMA_GET_TRANS_CNT(0);
//			bytecount1 = PDMA_GET_TRANS_CNT(1);
//			bytecount2 = PDMA_GET_TRANS_CNT(2);
		#endif
		CRITICAL_ERROR("wrong dma buffer state \n");
		return 1;
	}

	*supplied_tx_buffer_status = DMA_I9XXXX_BUFF_TX_DATA_IS_READY;
	num_of_buffers = cfg_hndl->num_of_buffers;
	num_of_prefilled_buffer_before_tx_start =
			cfg_hndl->num_of_prefilled_buffer_before_tx_start;
	next_supplied_tx_buffer = (next_supplied_tx_buffer + 1) % num_of_buffers;
	runtime_hndl->next_supplied_tx_buffer = next_supplied_tx_buffer;


	if (runtime_hndl->needed_full_dma_start)
	{
		prefilled_buffers = runtime_hndl->prefilled_buffers;
		prefilled_buffers++;
		runtime_hndl->prefilled_buffers = prefilled_buffers;

		if (prefilled_buffers < num_of_prefilled_buffer_before_tx_start )
		{
			return 0;
		}

		// (next_supplied_tx_buffer - 1) is location of last filled buffer
		next_dma_buff_indx = ( num_of_buffers + ((next_supplied_tx_buffer - 1) -
					num_of_prefilled_buffer_before_tx_start)) % num_of_buffers;
		runtime_hndl->curr_dma_buff_indx = next_dma_buff_indx;
		peripheral_type = cfg_hndl->peripheral_type;
		channel_num = cfg_hndl->channel_num;
		buffers = runtime_hndl->buff;
		peripheral_dev = cfg_hndl->peripheral_dev;

		runtime_hndl->prefilled_buffers = 0;
		runtime_hndl->needed_full_dma_start = 0;

		PDMA_SET_TRANS_CNT(channel_num,
				runtime_hndl->buff_size_in_transfer_words);
		PDMA_SET_SRC_ADDR(channel_num, (size_t)buffers[next_dma_buff_indx]);
		PDMA_SetTransferMode( channel_num, peripheral_type, FALSE, 0);
		enable_peripheral_output(peripheral_type, peripheral_dev, channel_num);
	}
	return 0;
}


/**
 * dma_i9xxxx_ioctl()
 *
 * return:
 */
uint8_t dma_i9xxxx_ioctl( struct dev_desc_t *adev, const uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	uint8_t ret;
	struct dma_i9xxxx_cfg_t *cfg_hndl;
	struct dma_i9xxxx_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(dma_i9xxxx, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(dma_i9xxxx, adev);

	ret = 0;
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		ret = start_dma_i9xxxx_device(adev, cfg_hndl, runtime_hndl);
		break;
	case DMA_I9XXXX_IOCTL_GET_RX_BUFF :
		ret = get_full_rx_buffer(cfg_hndl,
				runtime_hndl, aIoctl_param1, aIoctl_param2);
		break;
	case DMA_I9XXXX_IOCTL_RELEASE_RX_BUFF :
		ret = release_rx_buffer(cfg_hndl, runtime_hndl);
		break;
	case DMA_I9XXXX_IOCTL_GET_EMPTY_TX_BUFF :
		ret = get_empty_tx_buffer(adev, cfg_hndl,
				runtime_hndl, aIoctl_param1, aIoctl_param2);
		break;
	case DMA_I9XXXX_IOCTL_RELEASE_TX_BUFF :
		ret = release_tx_buffer(cfg_hndl, runtime_hndl);
		break;
	default :
		return 1;
	}

	return ret;
}

#define	MODULE_NAME                         dma_i9xxxx
#define	MODULE_IOCTL_FUNCTION               dma_i9xxxx_ioctl
#include "add_module.h"
