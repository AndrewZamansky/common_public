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

#include "ISD94XXXSeries.h"

#include "irq_api.h"
#include "clock_control_i94xxx_api.h"

#include "i2c.h"

/********  defines *********************/


/********  types  *********************/


/* ------------ External variables ---------------------------------*/

/* ------------------------ External functions ------------*/

/* ------------------------ Exported variables --------*/


/*--------------------------------------------------------------------------*/
/*  I2C TRx Callback Function                                                                               */
/*--------------------------------------------------------------------------*/
void I2C_SlaveTRx(uint32_t u32Status, I2C_T *i2c, struct dev_desc_t *adev)
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

//			I2C_SET_DATA(i2c, g_au8SlvTxData[module][slave_buff_addr[module]]);
//			slave_buff_addr[module]++;
//
//			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0xB8)
	{/* Slave Transmit Data ACK, master wants more data */
//			I2C_SET_DATA(i2c, g_au8SlvTxData[module][slave_buff_addr[module]]);
//			slave_buff_addr[module]++;
//
//			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0xC0)
	{
		 /* Data byte or last data in I2CDAT has been transmitted
		  *  Not ACK has been received */
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
		struct dev_desc_t * callback_rx_dev ;
		callback_rx_dev = cfg_hndl->callback_rx_dev;

		DEV_CALLBACK_2_PARAMS(callback_rx_dev,
				CALLBACK_DATA_RECEIVED,  in_buff,  (void*)curr_data_pos);
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
	else
	{
			/* TO DO */
		//	LOG_MSG("Status 0x%x is NOT processed\n", u32Status);
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
       	I2C_SlaveTRx(u32Status, i2c_regs, adev);
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
//	I2C_T *i2c_regs;
//	struct i2c_i94xxx_cfg_t *cfg_hndl;
//
//	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
//	i2c_regs =(I2C_T *)cfg_hndl->base_address;
//
//	I2C_WRITE(i2c_regs, *apData);
//    I2C_EnableInt(i2c_regs,  I2C_INTEN_THREIEN_Msk );

	return aLength;

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
			switch (cfg_hndl->pinout)
			{
			case I2C_I94XXX_I2C1_SCL_SDA_PINS_PORT_D_PINS_2_1:
			    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD1MFP_Msk);
			    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD2MFP_Msk);
			    SYS->GPD_MFPL |=  (0x02UL<<SYS_GPD_MFPL_PD1MFP_Pos);
			    SYS->GPD_MFPL |=  (0x04UL<<SYS_GPD_MFPL_PD2MFP_Pos);
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

	    /* Set I2C 4 Slave Addresses */
	    I2C_SetSlaveAddr(i2c_regs, 0, (cfg_hndl->slave_address) >> 1 , 0);

	    /* Set I2C 4 Slave Addresses Mask */
	    I2C_SetSlaveAddrMask(i2c_regs, 0, 0x01);

	    /* I2C enter no address SLV mode */
	    I2C_SET_CONTROL_REG(i2c_regs, I2C_CTL_SI_AA);

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

	default :
		return 1;
	}
	return 0;
}
