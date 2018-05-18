

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

/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/

#ifdef ENABLE_I94XX_SPI_INT

static os_queue_t xTX_WaitQueue, xRX_WaitQueue ;
static uint8_t dummy_msg;
volatile size_t delay;

/* Callback that Interrupt utilizes for read and write functions */

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

	/* Check RX EMPTY flag */
	if(SPI_GET_RX_FIFO_EMPTY_FLAG(spi_regs) == 0)
	{
		uint8_t u8InChar;
		uint8_t *rx_buff;
		volatile uint32_t bytes_left_to_read;

		rx_buff = runtime_handle->rx_buff;
		bytes_left_to_read = runtime_handle->bytes_left_to_read;
		u8InChar = SPI_READ_RX(spi_regs);
		/* Read RX FIFO */
		if(bytes_left_to_read)
		{
			*rx_buff++ = u8InChar;
			runtime_handle->rx_buff = rx_buff;
			bytes_left_to_read--;
			runtime_handle->bytes_left_to_read = bytes_left_to_read;
			SPI_WRITE_TX(spi_regs, 0xFF);
		}
		else if(NULL != rx_buff)
		{
			*rx_buff = u8InChar;
			 runtime_handle->rx_buff = rx_buff;
			if (NULL != xRX_WaitQueue)
			{
				os_queue_send_immediate( xRX_WaitQueue, ( void * ) &dummy_msg);
			}
		}


	}

	/* Check TX FULL flag and TX data count */
	if (SPI_GET_TX_FIFO_FULL_FLAG(spi_regs) == 0)
	{
		uint8_t  const *tx_buff;
		volatile uint32_t bytes_left_to_transmit;

		bytes_left_to_transmit = runtime_handle->bytes_left_to_transmit;
		tx_buff = runtime_handle->tx_buff;

		if (bytes_left_to_transmit)
		{
			SPI_WRITE_TX(spi_regs, *tx_buff++);
			bytes_left_to_transmit--;
			runtime_handle->bytes_left_to_transmit = bytes_left_to_transmit;
			runtime_handle->tx_buff = tx_buff;
		}
		else if (NULL != xTX_WaitQueue)
		{
			os_queue_send_immediate( xTX_WaitQueue, ( void * ) &dummy_msg);
			//break;
		}
	}
	SPI_CLR_UNIT_TRANS_INT_FLAG(spi_regs);

    return 0;
}
#endif




/**
 * spi_i94xxx_pread()
 *
 * return:
 */
size_t spi_i94xxx_pread(struct dev_desc_t *adev,
			uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct spi_i94xxx_cfg_t *cfg_hndl;
	struct spi_i94xxx_runtime_t *runtime_handle;
	SPI_T *spi_regs;

	// Acquire handles and spi base address
	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	spi_regs = (SPI_T *)cfg_hndl->base_address;

	// Set up the bytes to read and get the rx_buff pointer
	//  to be
	if (0 == aLength) return 0;
	else runtime_handle->bytes_left_to_read = aLength - 1;
	runtime_handle->rx_buff = apData;

	SPI_WRITE_TX(spi_regs, 0xFF);
	os_queue_receive_infinite_wait( xRX_WaitQueue ,  &dummy_msg  );

	runtime_handle->rx_buff = NULL;
	return aLength;

}
/**
 * spi_i94xxx_pwrite()
 *
 * return:
 */
size_t spi_i94xxx_pwrite(struct dev_desc_t *adev ,
		const uint8_t *apData , size_t aLength, size_t aOffset)
{
	struct spi_i94xxx_cfg_t *cfg_hndl;
	struct spi_i94xxx_runtime_t *runtime_handle;
	SPI_T *spi_regs;

	if (0 == aLength) return 0;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	spi_regs = (SPI_T *)cfg_hndl->base_address;

#if 0
	while ((0 == SPI_GET_TX_FIFO_FULL_FLAG(spi_regs) ) && (aLength) )
	{
		SPI_WRITE_TX(spi_regs, *apData++);
		aLength--;
//		apData++;
	}
	runtime_handle->tx_buff = &apData;
	runtime_handle->bytes_left_to_transmit = aLength;
#else
	runtime_handle->tx_buff = &apData[1];
	runtime_handle->bytes_left_to_transmit = aLength - 1;

	SPI_WRITE_TX(spi_regs, *apData);
#endif
	//delay needed by ArduinoSDSPI due to SD card not being able to respond
	// quickly. Change if needed.
	delay = 1100;
	while (delay--);
	os_queue_receive_infinite_wait( xTX_WaitQueue ,  &dummy_msg  );
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

	struct dev_desc_t	*spi_clk_dev;
	struct dev_desc_t	*src_clock;
	uint32_t spi_module_rst;
	uint32_t clk_freq;
	SPI_T *spi_regs;
	int spi_irq;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
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

			SPI_SET_DATA_WIDTH(spi_regs, cfg_hndl->data_width);

			#ifdef ENABLE_I94XX_SPI_INT
			/* Config Suspend cycle */
			SPI_SET_SUSPEND_CYCLE(spi_regs, 5);

			/* Enable UART RDA/RLS/Time-out interrupt */
			SPI_EnableInt(spi_regs, SPI_UNIT_INT_MASK);

			irq_register_device_on_interrupt(spi_irq, adev);
//			irq_set_priority(spi_irq, INTERRUPT_LOWEST_PRIORITY - 1 );
			irq_set_priority(spi_irq, OS_MAX_INTERRUPT_PRIORITY_FOR_API_CALLS );
			irq_enable_interrupt(spi_irq);

			SPI_ENABLE(spi_regs);
			#endif

			xTX_WaitQueue = os_create_queue( 1 , sizeof(uint8_t ) );
			xRX_WaitQueue = os_create_queue( 1 , sizeof(uint8_t ) );

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
