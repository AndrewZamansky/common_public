

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
/*#define SPI_SET_SS_HIGH(spi)   ((spi)->SSCTL = ((spi)->SSCTL & \
     (~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS_Msk)))) */
#define SPI_SET_SS_HIGH_SPI0()     ((SPI0)->SSCTL = ((SPI0)->SSCTL & \
		(~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS0_Msk))))
#define SPI_SET_SS_HIGH_SPI1()     ((SPI1)->SSCTL = ((SPI1)->SSCTL & \
		(~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk | SPI_SSCTL_SS1_Msk))))

/*#define SPI_SET_SS_LOW(spi)   ((spi)->SSCTL = ((spi)->SSCTL & \
      (~(SPI_SSCTL_AUTOSS_Msk | SPI_SSCTL_SSACTPOL_Msk))) | SPI_SSCTL_SS_Msk)*/
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

//#define BUFF_SIZE	5000
//static uint8_t tmp_buf[BUFF_SIZE] = { [0 ... 4999] = 0xcc };
//uint32_t tmp_buf_pos = 0;

uint8_t spi_i94xxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct spi_i94xxx_cfg_t *cfg_hndl;
	struct spi_i94xxx_runtime_t *runtime_handle;

    uint8_t u8InChar;
	SPI_T *spi_regs;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	spi_regs =(SPI_T *)cfg_hndl->base_address;

	/* Check RX EMPTY flag */
	if(SPI_GET_RX_FIFO_EMPTY_FLAG(spi_regs) == 0)
	{
		uint8_t *rx_buff;
		uint32_t curr_rx_pos;

		rx_buff = runtime_handle->rx_buff;
		curr_rx_pos = runtime_handle->curr_rx_pos;
		/* Read RX FIFO */
		u8InChar = SPI_READ_RX(spi_regs);
		if (NULL != rx_buff)
		{
			rx_buff[curr_rx_pos++] = u8InChar;
//			if (BUFF_SIZE > tmp_buf_pos)
//			{
//				tmp_buf[tmp_buf_pos++] = u8InChar;
//			}
		}
		runtime_handle->curr_rx_pos = curr_rx_pos;
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
	volatile uint32_t curr_rx_pos;
	uint32_t next_rx_pos;
	SPI_T *spi_regs;

	if (0 == aLength) return 0;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	spi_regs = (SPI_T *)cfg_hndl->base_address;

	runtime_handle->curr_rx_pos = 0;
	runtime_handle->rx_buff = apData;
	curr_rx_pos = 0;
	next_rx_pos = 1;
//	while (0 == SPI_GET_RX_FIFO_EMPTY_FLAG(spi_regs))
//	{
//		uint8_t dummy;
//		dummy = SPI_READ_RX(spi_regs);
//	}
	while ( curr_rx_pos < aLength)
	{
		SPI_WRITE_TX(spi_regs, 0xFF);
		while(curr_rx_pos != next_rx_pos)
		{
			os_delay_ms(1);
			curr_rx_pos = runtime_handle->curr_rx_pos;
		}
		next_rx_pos++;
	}
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
	volatile uint32_t bytes_left_to_transmit;
	SPI_T *spi_regs;

	if (0 == aLength) return 0;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	spi_regs = (SPI_T *)cfg_hndl->base_address;

	bytes_left_to_transmit = aLength;
	runtime_handle->tx_buff = &apData[1];
	runtime_handle->bytes_left_to_transmit = aLength - 1;
	SPI_WRITE_TX(spi_regs, *apData);
	while (bytes_left_to_transmit)
	{
		os_delay_ms(2);
		bytes_left_to_transmit = runtime_handle->bytes_left_to_transmit;
	}
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
			irq_set_priority(spi_irq, INTERRUPT_LOWEST_PRIORITY - 1 );
			irq_enable_interrupt(spi_irq);

			SPI_ENABLE(spi_regs);
			#endif

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
