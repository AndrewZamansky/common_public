/*
 *
 * i2c_i94xxx.c

 *
 */

/********  includes *********************/

#include "i2c_i94xxx.h"

#include "i2c_api.h"
#include "i2c_i94xxx_add_component.h"
#include "_i2c_i94xxx_prerequirements_check.h"
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "I94100.h"

#include "irq_api.h"
#include "clock_control_i94xxx_api.h"

#include "i2c.h"

/********  defines *********************/


/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/

static void transmit_byte(I2C_T *i2c, struct i2c_i94xxx_cfg_t *cfg_hndl,
		struct i2c_i94xxx_runtime_t *runtime_handle)
{
	size_t tx_data_size;
	uint8_t  const *tx_data;

	tx_data_size = runtime_handle->tx_data_size;
	if (tx_data_size)
	{
		tx_data = runtime_handle->tx_data;
		I2C_SET_DATA(i2c, *tx_data++);
		runtime_handle->tx_data = tx_data;
		tx_data_size--;
		runtime_handle->tx_data_size = tx_data_size;
		runtime_handle->transmitted_data_size++;
	}
	else
	{
		if (I2C_I94XXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
		{
			I2C_SET_DATA(i2c, 0x00);
		}
		else
		{
			I2C_SET_CONTROL_REG(I2C1, I2C_CTL_STO_SI);
		}
	}
	if (I2C_I94XXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
	{
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
	else
	{
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
	}
}

static void end_of_transmition(struct i2c_i94xxx_cfg_t *cfg_hndl,
		struct i2c_i94xxx_runtime_t *runtime_handle)
{
	size_t   transmitted_data_size;

	transmitted_data_size = runtime_handle->transmitted_data_size;

	if (transmitted_data_size)
	{
		struct dev_desc_t *callback_tx_dev;

		callback_tx_dev = cfg_hndl->callback_tx_dev;
		if (NULL != callback_tx_dev)
		{
			DEV_CALLBACK_1_PARAMS(callback_tx_dev ,
					CALLBACK_TX_DONE, (void*)transmitted_data_size);
			runtime_handle->transmitted_data_size = 0;
		}
	}
}

/*
 * function : I2C_SlaveTRx
 *
 */
static void I2C_SlaveTRx(
		uint32_t u32Status, I2C_T *i2c, struct dev_desc_t *adev)
{
	uint8_t *in_buff;
	struct i2c_i94xxx_runtime_t *runtime_handle;
	struct i2c_i94xxx_cfg_t *cfg_hndl;
	uint16_t curr_data_pos;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

	in_buff = runtime_handle->rcv_data;
	curr_data_pos = runtime_handle->curr_data_pos;

	if(u32Status == 0x60)
	{/* Own SLA+W has been receive; ACK has been return */
		runtime_handle->curr_data_pos = 0;
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0x80)
	{
		/* Previously address with own SLA address
		Data has been received; ACK has been returned*/
		uint8_t data;


		if (I2C_I94XXX_RCV_DATA_SIZE_BUFFER > curr_data_pos)
		{
			data = (unsigned char) I2C_GET_DATA(i2c);
			in_buff[curr_data_pos++] = data;
			runtime_handle->curr_data_pos = curr_data_pos;

			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
		}
		else
		{
			// TODO
		}
	}
	else if(u32Status == 0xA8)
	{/* Own SLA+R has been receive; ACK has been return */
		transmit_byte(i2c, cfg_hndl, runtime_handle);
	}
	else if(u32Status == 0xB8)
	{/* Slave Transmit Data ACK, master wants more data */
		transmit_byte(i2c, cfg_hndl, runtime_handle);
	}
	else if(u32Status == 0xC0)
	{
		 /* Data byte or last data in I2CDAT has been transmitted
		  *  Not ACK has been received */
		end_of_transmition(cfg_hndl, runtime_handle);
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0x88)
	{
		/* Previously addressed with own SLA address; NOT ACK has
						 been returned */
		//	g_u8SlvDataLen[module] = 0;
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0xA0)
	{
		/* A STOP or repeated START has been received while still
			addressed as Slave/Receiver*/
		struct dev_desc_t *callback_rx_dev ;

		callback_rx_dev = cfg_hndl->callback_rx_dev;

		if (NULL != callback_rx_dev)
		{
			DEV_CALLBACK_2_PARAMS(callback_rx_dev,
				CALLBACK_DATA_RECEIVED, in_buff, (void*)(size_t)curr_data_pos);
		}
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
	else
	{
			/* TO DO */
		//	LOG_MSG("Status 0x%x is NOT processed\n", u32Status);
	}

}

/*
 * function : I2C_MasterTRx
 *
 */
static void I2C_MasterTRx(
		uint32_t u32Status, I2C_T *i2c, struct dev_desc_t *adev)
{
//	uint8_t *in_buff;
	struct i2c_i94xxx_runtime_t *runtime_handle;
	struct i2c_i94xxx_cfg_t *cfg_hndl;
//	uint16_t curr_data_pos;
	uint8_t  notify_callback;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);
	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);

//	in_buff = runtime_handle->rcv_data;
//	curr_data_pos = runtime_handle->curr_data_pos;

	notify_callback = 0;
	switch(u32Status)
	{
	/* START has been transmitted and Write SLA+W to Register I2CDAT. */
	case 0x08:
		I2C_SET_DATA(I2C1, runtime_handle->remote_slave_addr << 1);
		I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
		break;
	/* SLA+W has been transmitted and ACK has been received. */
	case 0x18:
		transmit_byte(i2c, cfg_hndl, runtime_handle);
		notify_callback = 1;
		break;
	/* SLA+W has been transmitted and NACK has been received. */
	case 0x20:
		I2C_STOP(I2C1);
		I2C_START(I2C1);
		notify_callback = 1;
		break;
	/* DATA has been transmitted and ACK has been received. */
	case 0x28:
		transmit_byte(i2c, cfg_hndl, runtime_handle);
		notify_callback = 1;
		break;
	}

	if ( notify_callback && (0 == runtime_handle->tx_data_size))
	{
		end_of_transmition(cfg_hndl, runtime_handle);
	}
}

uint8_t i2c_i94xxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct i2c_i94xxx_cfg_t *cfg_hndl;
	uint32_t u32Status;
	I2C_T *i2c_regs;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	i2c_regs =(I2C_T *)cfg_hndl->base_address;


	u32Status = I2C_GET_STATUS(i2c_regs);
	if(I2C_GET_TIMEOUT_FLAG(i2c_regs))
	{
		/* Clear I2C0 Timeout Flag */
		I2C_ClearTimeoutFlag(i2c_regs);
	}
	else
	{
		if (I2C_I94XXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
		{
			I2C_SlaveTRx(u32Status, i2c_regs, adev);
		}
		else
		{
			I2C_MasterTRx(u32Status, i2c_regs, adev);
		}
	}

	return 0;
}


/**
 * i2c_i94xxx_pwrite()
 *
 * return:
 */
size_t i2c_i94xxx_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct i2c_i94xxx_cfg_t *cfg_hndl;
	struct i2c_i94xxx_runtime_t *runtime_handle;
	size_t   tx_data_size;
	size_t   transmitted_data_size;
	uint8_t  const *tx_data;
//	I2C_T *i2c_regs;
//	struct i2c_i94xxx_cfg_t *cfg_hndl;

	if (0 == aLength)
	{
		return 0;
	}
	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adev);

	if (I2C_I94XXX_API_MASTER_MODE == cfg_hndl->master_slave_mode)
	{
		// first byte is address of slave
		tx_data_size = aLength - 1;
		transmitted_data_size = 1;
		tx_data = &apData[1];
		runtime_handle->remote_slave_addr = apData[0];
		I2C_START(I2C1);
	}
	else
	{
		tx_data_size = aLength;
		transmitted_data_size = 0;
		tx_data = apData;
	}
	runtime_handle->tx_data = tx_data;
	runtime_handle->tx_data_size = tx_data_size;
	runtime_handle->transmitted_data_size = transmitted_data_size;
//	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
//	i2c_regs =(I2C_T *)cfg_hndl->base_address;
//
//	I2C_WRITE(i2c_regs, *apData);
//	I2C_EnableInt(i2c_regs,  I2C_INTEN_THREIEN_Msk );

	return 0;

}


/**
 * i2c_i94xxx_ioctl()
 *
 * return:
 */
uint8_t i2c_i94xxx_ioctl( struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct i2c_i94xxx_cfg_t *cfg_hndl;
	I2C_T *i2c_regs;
	int i2c_irq;
	uint32_t i2c_module_rst;
//	struct dev_desc_t	*src_clock;
	struct dev_desc_t	*i2c_clk_dev;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	i2c_regs = (I2C_T *)cfg_hndl->base_address;
//	src_clock = cfg_hndl->src_clock;
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		if(I2C1 == i2c_regs)
		{
			i2c_irq = I2C1_IRQn;
			i2c_module_rst = I2C1_MODULE;
			i2c_clk_dev = i94xxx_i2c1_clk_dev;
			//i2c_clk_src = CLK_I2C0_SRC_EXT;
			/*
			* Init I/O Multi-function
			* Set PA multi-function pins for I2C0 RXD(PA.8) and TXD(PA.7)
			*/
			switch (cfg_hndl->SCL_pinout)
			{
			case I2C_I94XXX_API_I2C1_SCL_PIN_PORT_D_PIN_2:
				SYS->GPD_MFPL &= ~SYS_GPD_MFPL_PD2MFP_Msk;
				SYS->GPD_MFPL |=  (0x04UL<<SYS_GPD_MFPL_PD2MFP_Pos);
				break;
			case I2C_I94XXX_API_I2C1_SCL_PIN_PORT_D_PIN_14:
				SYS->GPD_MFPH &= ~SYS_GPD_MFPH_PD14MFP_Msk;
				SYS->GPD_MFPH |=  SYS_GPD_MFPH_PD14MFP_I2C1_SCL;
				break;
			default :
				return 1;
			}

			switch (cfg_hndl->SDA_pinout)
			{
			case I2C_I94XXX_API_I2C1_SDA_PIN_PORT_D_PIN_1:
				SYS->GPD_MFPL &= ~SYS_GPD_MFPL_PD1MFP_Msk;
				SYS->GPD_MFPL |=  SYS_GPD_MFPL_PD1MFP_I2C1_SDA;
				break;
			case I2C_I94XXX_API_I2C1_SDA_PIN_PORT_D_PIN_15:
				SYS->GPD_MFPH &= ~SYS_GPD_MFPH_PD15MFP_Msk;
				SYS->GPD_MFPH |=  SYS_GPD_MFPH_PD15MFP_I2C1_SDA;
				break;
			default :
				return 1;
			}
		}
		else
		{
			return 1;
		}


		DEV_IOCTL_0_PARAMS(i2c_clk_dev, CLK_IOCTL_ENABLE);

		/* Reset I2C module */
		SYS_ResetModule(i2c_module_rst);

		/* Configure I2C and set I2C baud rate */
		I2C_Open(i2c_regs, cfg_hndl->baud_rate);

		if (I2C_I94XXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
		{
			I2C_SetSlaveAddr(i2c_regs, 0, (cfg_hndl->slave_address) >> 1 , 0);
			I2C_SetSlaveAddrMask(i2c_regs, 0, 0x01);

			/* I2C enter no address SLV mode */
			I2C_SET_CONTROL_REG(i2c_regs, I2C_CTL_SI_AA);
		}

		I2C_EnableInt(i2c_regs);

		irq_register_device_on_interrupt(i2c_irq, adev);
		irq_set_priority(i2c_irq, INTERRUPT_LOWEST_PRIORITY - 1 );
		irq_enable_interrupt(i2c_irq);
		break;

	case IOCTL_I2C_DISABLE_TX :
		break;

	case IOCTL_I2C_ENABLE_TX :
		break;

	case IOCTL_I2C_SET_ISR_CALLBACK_TX_DEV:
		cfg_hndl->callback_tx_dev =(struct dev_desc_t *) aIoctl_param1;
		break;

	case IOCTL_I2C_SET_ISR_CALLBACK_RX_DEV:
		cfg_hndl->callback_rx_dev =(struct dev_desc_t *) aIoctl_param1;
		break;
	case IOCTL_I2C_SET_MASTER_CLOCK_RATE_HZ:
		I2C_SetBusClockFreq(i2c_regs, *(uint32_t*)aIoctl_param1);
	default :
		return 1;
	}
	return 0;
}
