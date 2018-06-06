

/*
 *
 * spi_i94xxx.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"



#include "os_wrapper.h"

#include "I94100.h"
#include "irq_api.h"

#include "spi.h"

#include "_spi_i94xxx_prerequirements_check.h"
#include "clock_control_i94xxx_api.h"

/*following line add module to available module list for dynamic device tree*/
#include "spi_i94xxx_add_component.h"


/********  defines *********************/
#define ENABLE_I94XX_SPI_INT

/* replace original SPI_SET_SS_HIGH / SPI_SET_SS_LOW because SPI_SSCTL_SS_Msk
 * is undefined
 */
#define SPI_SET_SS_HIGH_SPI0()     ((SPI0)->SSCTL = ((SPI0)->SSCTL & \
		(~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS0_Msk))))
#define SPI_SET_SS_HIGH_SPI1()     ((SPI1)->SSCTL = ((SPI1)->SSCTL & \
		(~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS1_Msk))))

#define SPI_SET_SS_LOW_SPI0()     ((SPI0)->SSCTL = ((SPI0)->SSCTL & \
		(~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk))) | SPI_SSCTL_SS0_Msk)
#define SPI_SET_SS_LOW_SPI1()     ((SPI1)->SSCTL = ((SPI1)->SSCTL & \
		(~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk))) | SPI_SSCTL_SS1_Msk)

/**
  * @brief      Get the count of available data in TX FIFO.
  * @param[in]  spi The pointer of the specified SPI module.
  * @return     The count of available data in TX FIFO.
  * @details    Read TXCNT(SPI_STATUS[27:24]) the count of available data in TX FIFO.
  */
#define SPI_GET_TX_FIFO_COUNT(spi)   	\
		(((spi)->STATUS & SPI_STATUS_TXCNT_Msk) >> SPI_STATUS_TXCNT_Pos)

/**
  * @brief      Get the RX FIFO full flag.
  * @param[in]  spi The pointer of the specified SPI module.
  * @retval     0 RX FIFO is not full.
  * @retval     1 RX FIFO is full.
  * @details    Read RXFULL bit flag of SPI_STATUS register
  */
#define SPI_GET_RX_FIFO_FULL_FLAG(spi)	\
		(((spi)->STATUS & SPI_STATUS_RXFULL_Msk) >> SPI_STATUS_RXFULL_Pos)

/********  types  *********************/


/* ------------------------ External variables ------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/

#ifdef ENABLE_I94XX_SPI_INT

static uint8_t dummy_msg;


static void spi_i94xxx_pwrite_callback(SPI_T *spi_regs,
					struct spi_i94xxx_runtime_t *runtime_handle)
{
	volatile uint32_t bytes_left_to_transmit;
	uint8_t const *tx_buff;
	os_queue_t xTX_WaitQueue;

	tx_buff = runtime_handle->tx_buff;

	if (NULL == tx_buff)
	{
		return;
	}

	xTX_WaitQueue = runtime_handle->xTX_WaitQueue;
	bytes_left_to_transmit = runtime_handle->bytes_left_to_transmit;

	while (bytes_left_to_transmit && (0 == SPI_GET_TX_FIFO_FULL_FLAG(spi_regs)))
	{
		SPI_WRITE_TX(spi_regs, *tx_buff++);
		bytes_left_to_transmit--;
	}

	if (0 == bytes_left_to_transmit)
	{
		SPI_DisableInt(spi_regs, SPI_FIFO_TXTH_INT_MASK);
		SPI_EnableInt(spi_regs, SPI_UNIT_INT_MASK);

		if (SPI_GET_TX_FIFO_EMPTY_FLAG(spi_regs) && !SPI_IS_BUSY(spi_regs))
		{
			tx_buff = NULL;
			if (NULL != xTX_WaitQueue)
			{
				os_queue_send_immediate( xTX_WaitQueue, ( void * ) &dummy_msg);
			}
		}
	}
	runtime_handle->bytes_left_to_transmit = bytes_left_to_transmit;
	runtime_handle->tx_buff = tx_buff;
}

static void spi_i94xxx_pread_callback(SPI_T *spi_regs,
					struct spi_i94xxx_runtime_t *runtime_handle)
{
	uint8_t *rx_buff;
	volatile uint32_t bytes_left_to_read;
	os_queue_t xRX_WaitQueue;

	rx_buff = runtime_handle->rx_buff;

	if (NULL == rx_buff)
	{
		SPI_ClearRxFIFO(spi_regs);
		return;
	}

	xRX_WaitQueue = runtime_handle->xRX_WaitQueue;
	bytes_left_to_read = runtime_handle->bytes_left_to_read;


	// Fill the TX FIFO with the amount we wish to read
	while (bytes_left_to_read && (6 > SPI_GET_TX_FIFO_COUNT(spi_regs)))
	{

		while (0 == SPI_GET_RX_FIFO_EMPTY_FLAG(spi_regs))
		{
			*rx_buff++ = SPI_READ_RX(spi_regs);
		}
		SPI_WRITE_TX(spi_regs, 0xFF);
		bytes_left_to_read--;
		// Read from RX FIFO until empty

	}



	if (0 == bytes_left_to_read)
	{
		SPI_DisableInt(spi_regs, SPI_FIFO_TXTH_INT_MASK);
		SPI_EnableInt(spi_regs, SPI_UNIT_INT_MASK);
		if (SPI_GET_TX_FIFO_EMPTY_FLAG(spi_regs) && !SPI_IS_BUSY(spi_regs))
		{
			// Read from RX FIFO until empty
			while (0 == SPI_GET_RX_FIFO_EMPTY_FLAG(spi_regs))
			{
				*rx_buff++ = SPI_READ_RX(spi_regs);
			}
			rx_buff = NULL;
			if (NULL != xRX_WaitQueue)
			{
				os_queue_send_immediate( xRX_WaitQueue, ( void * ) &dummy_msg);
			}
		}
	}

	runtime_handle->rx_buff = rx_buff;
	runtime_handle->bytes_left_to_read = bytes_left_to_read;
}

/**
 * spi_i94xxx_callback()
 *
 * Initial function that is called during spi read. This function starts an
 *		interrupt scheme that triggers on FIFO threshold, that needs to be
 *		specified in the Device Tree, located in the main folder.
 *
 * return: aLength minus number of bytes read. Should be 0.
 */

uint8_t spi_i94xxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{

	struct spi_i94xxx_cfg_t *cfg_hndl;
	struct spi_i94xxx_runtime_t *runtime_handle;

	SPI_T *spi_regs;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	spi_regs =(SPI_T *)cfg_hndl->base_address;

	//Clear all flags before read/write to prevent previous flag
	// from interfering with flag that would be set by sending.
	SPI_CLR_UNIT_TRANS_INT_FLAG(spi_regs);
	spi_regs->STATUS = SPI_STATUS_TXTHIF_Msk;

	spi_i94xxx_pread_callback(spi_regs, runtime_handle);

	spi_i94xxx_pwrite_callback(spi_regs, runtime_handle);



    return 0;
}
#endif




/**
 * spi_i94xxx_pread()
 *
 * Standardized function that follows uProject protocol (adev) that requires a
 * 		specific input structure:
 *
 * 	struct dev_desc_t *adev - Device structure to be used, defined in .h file
 * 	uint8_t *apData			- Address of data to be either transmitted or
 * 								received
 * 	size_t aLength
 * 	size_t aOffset
 *
 * Initial function that is called during spi read. This function starts an
 *		interrupt scheme that triggers on FIFO threshold, that needs to be
 *		specified in the Device Tree, located in the main folder.
 *
 * return: size_t aLength minus number of bytes read. Should be 0.
 */
size_t spi_i94xxx_pread(struct dev_desc_t *adev,
			uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct spi_i94xxx_cfg_t *cfg_hndl;
	struct spi_i94xxx_runtime_t *runtime_handle;

	SPI_T *spi_regs;
	os_queue_t xRX_WaitQueue;
	os_mutex_t  spi_mutex;

	// Acquire handles and spi base address
	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	spi_regs = (SPI_T *)cfg_hndl->base_address;
	xRX_WaitQueue = runtime_handle->xRX_WaitQueue;
	spi_mutex = runtime_handle->spi_mutex;

	os_mutex_take_infinite_wait(spi_mutex);

	//Clear the RX FIFO from previous writes
	SPI_ClearRxFIFO(spi_regs);

	if (0 == aLength) return 0;

	runtime_handle->bytes_left_to_read = aLength - 1;
	runtime_handle->rx_buff = apData;


	SPI_DisableInt(spi_regs, SPI_UNIT_INT_MASK);

	SPI_WRITE_TX(spi_regs, 0xFF);

	SPI_EnableInt(spi_regs, SPI_FIFO_TXTH_INT_MASK);

	os_queue_receive_infinite_wait( xRX_WaitQueue ,  &dummy_msg  );

	os_mutex_give(spi_mutex);

	return aLength;

}

/**
 * spi_i94xxx_pwrite()
 *
 * Standardized function that follows uProject protocol that requires the
 * 		specific input structure:
 * 	(struct dev_desc_t *adev, uint8_t *apData, size_t aLength, size_t aOffset)
 *
 * Initial function that is called during spi write. This function starts an
 *		interrupt scheme that triggers on FIFO threshold, that needs to be
 *		specified in the Device Tree, located in the main folder.
 * return:
 */
size_t spi_i94xxx_pwrite(struct dev_desc_t *adev ,
		const uint8_t *apData , size_t aLength, size_t aOffset)
{
	struct spi_i94xxx_cfg_t *cfg_hndl;
	struct spi_i94xxx_runtime_t *runtime_handle;
	SPI_T *spi_regs;
	os_queue_t xTX_WaitQueue;
	os_mutex_t  spi_mutex;

	if (0 == aLength) return 0;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	spi_regs = (SPI_T *)cfg_hndl->base_address;
	xTX_WaitQueue = runtime_handle->xTX_WaitQueue;
	spi_mutex = runtime_handle->spi_mutex;

	os_mutex_take_infinite_wait(spi_mutex);

	SPI_DisableInt(spi_regs, SPI_UNIT_INT_MASK);

	runtime_handle->tx_buff = &apData[1];
	runtime_handle->bytes_left_to_transmit = aLength - 1;
	SPI_WRITE_TX(spi_regs, *apData);

	SPI_EnableInt(spi_regs, SPI_FIFO_TXTH_INT_MASK);

	os_queue_receive_infinite_wait( xTX_WaitQueue ,  &dummy_msg  );

	os_mutex_give(spi_mutex);

	return aLength;
}




static void configure_spi0_pinout(struct spi_i94xxx_cfg_t *cfg_hndl)
{
	if (SPI_I94XXX_API_CLK_PIN_C12 == cfg_hndl->CLK_pin)
	{
		SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC12MFP_Msk);
		SYS->GPC_MFPH |= SYS_GPC_MFPH_PC12MFP_SPI0_CLK;
	}

	if (SPI_I94XXX_API_MISO_PIN_C11 == cfg_hndl->MISO_pin)
	{
		SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC11MFP_Msk);
		SYS->GPC_MFPH |= SYS_GPC_MFPH_PC11MFP_SPI0_MISO0;
	}

	if (SPI_I94XXX_API_MOSI_PIN_C10 == cfg_hndl->MOSI_pin)
	{
		SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC10MFP_Msk);
		SYS->GPC_MFPH |= SYS_GPC_MFPH_PC10MFP_SPI0_MOSI0;
	}

	if (SPI_I94XXX_API_SS_PIN_C7 == cfg_hndl->SS_pin)
	{
		SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC7MFP_Msk);
		SYS->GPC_MFPL |= SYS_GPC_MFPL_PC7MFP_SPI0_SS0;
	}

	//Secondary pinout for SPI0

	if (SPI_I94XXX_API_CLK_PIN_A5 == cfg_hndl->CLK_pin)
	{
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA5MFP_Msk);
		SYS->GPA_MFPL |= SYS_GPA_MFPL_PA5MFP_SPI0_CLK;
	}

	if (SPI_I94XXX_API_MISO_PIN_A4 == cfg_hndl->MISO_pin)
	{
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA4MFP_Msk);
		SYS->GPA_MFPL |= SYS_GPA_MFPL_PA4MFP_SPI0_MISO0;
	}

	if (SPI_I94XXX_API_MOSI_PIN_A3 == cfg_hndl->MOSI_pin)
	{
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA3MFP_Msk);
		SYS->GPA_MFPL |= SYS_GPA_MFPL_PA3MFP_SPI0_MOSI0;
	}

	if (SPI_I94XXX_API_SS_PIN_A6 == cfg_hndl->SS_pin)
	{
		SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA6MFP_Msk);
		SYS->GPA_MFPL |= SYS_GPA_MFPL_PA6MFP_SPI0_SS0;
	}

}



/**
 * spi_i94xxx_ioctl()
 *
 * return:
 */
uint8_t spi_i94xxx_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct spi_i94xxx_cfg_t *cfg_hndl;
	struct spi_i94xxx_runtime_t *runtime_handle;

	struct dev_desc_t	*spi_clk_dev;
	struct dev_desc_t	*src_clock;
	uint32_t spi_module_rst;
	uint32_t clk_freq;
	SPI_T *spi_regs;
	int spi_irq;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	src_clock = cfg_hndl->src_clock;
	spi_regs = (SPI_T *)cfg_hndl->base_address;

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :

		if(SPI0 == spi_regs)
		{
			spi_clk_dev = i94xxx_spi0_clk_dev;
			spi_module_rst = SPI0_RST;
			spi_irq = SPI0_IRQn;
			configure_spi0_pinout(cfg_hndl);
		}
		else
		{
			return 1;
		}

		DEV_IOCTL_1_PARAMS(spi_clk_dev, CLK_IOCTL_SET_PARENT, src_clock);
		DEV_IOCTL_0_PARAMS(spi_clk_dev, CLK_IOCTL_ENABLE);

		SYS_ResetModule(spi_module_rst);

		/* Configure SPI0 as a master, SPI clock rate 2 MHz,
		 *  clock idle low, 32-bit transaction, drive output on
		 * falling clock edge and latch input on rising edge. */
		SPI_Open(spi_regs, SPI_MASTER, SPI_MODE_0, 8, 250000);
		/* Enable the automatic hardware slave selection function.
		 *  Select the SPI0_SS pin and configure as low-active. */
		//SPI_EnableAutoSS(spi_regs, SPI_SS0, SPI_SS_ACTIVE_LOW);

		/* Config Data Width - Supports 8-32bit */
		SPI_SET_DATA_WIDTH(spi_regs, cfg_hndl->data_width);

		SPI_I2S_SET_TXTH(spi_regs, SPI_I2S_FIFO_TX_LEVEL_0);
		/* Config Suspend Cycle (arg# + 0.5) clock cycles  */
		SPI_SET_SUSPEND_CYCLE(spi_regs, 0);

		#ifdef ENABLE_I94XX_SPI_INT


		/* Enable UART RDA/RLS/Time-out interrupt */
//		SPI_EnableInt(spi_regs, SPI_UNIT_INT_MASK);

		irq_register_device_on_interrupt(spi_irq, adev);
		irq_set_priority(spi_irq, INTERRUPT_LOWEST_PRIORITY - 1 );
		irq_enable_interrupt(spi_irq);

		SPI_ENABLE(spi_regs);
		#endif

		runtime_handle->xTX_WaitQueue = os_create_queue( 1 , sizeof(uint8_t ) );
		runtime_handle->xRX_WaitQueue = os_create_queue( 1 , sizeof(uint8_t ) );
		runtime_handle->spi_mutex = os_create_mutex();

		break;

	case IOCTL_SPI_API_SET_CS_HIGH :
		if(SPI0 == spi_regs)
		{
			SPI_SET_SS_HIGH_SPI0();
		}
		else
		{
			SPI_SET_SS_HIGH_SPI1();
		}
		break;

	case IOCTL_SPI_API_SET_CS_LOW :
		if(SPI0 == spi_regs)
		{
			SPI_SET_SS_LOW_SPI0();
		}
		else
		{
			SPI_SET_SS_LOW_SPI1();
		}
		break;


	case IOCTL_SPI_API_SET_CLK :
		clk_freq = (uint32_t)aIoctl_param1;
		SPI_Open(spi_regs, SPI_MASTER, SPI_MODE_0, 8, clk_freq);
		SPI_SET_DATA_WIDTH(spi_regs, cfg_hndl->data_width);
		break;

	default :
		return 1;
	}
	return 0;
}
