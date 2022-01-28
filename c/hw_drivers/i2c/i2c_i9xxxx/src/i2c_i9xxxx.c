/*
 *
 * i2c_i9xxxx.c

 *
 */

#include "i2c_i9xxxx_api.h"
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "i2c_api.h"

#include "dev_management_api.h"

#ifdef CONFIG_I96XXX_M0
	#define i9xxxx_i2c0_clk_dev   i96xxx_i2c0_clk_dev
	#define i9xxxx_i2c1_clk_dev   NULL//i96xxx_i2c1_clk_dev
#elif defined(CONFIG_I94XXX)
	#define i9xxxx_i2c0_clk_dev   i94xxx_i2c0_clk_dev
	#define i9xxxx_i2c1_clk_dev   i94xxx_i2c1_clk_dev
#endif


#include "irq_api.h"
#include "pin_control_api.h"

#include "i2c.h"
#include "i2c_i9xxxx.h"



#if !defined(INTERRUPT_PRIORITY_FOR_I2C)
	#error "INTERRUPT_PRIORITY_FOR_I2C should be defined"
#endif

#if CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(INTERRUPT_PRIORITY_FOR_I2C)
	#error "priority should be lower then maximal priority for os syscalls"
#endif


#define  NUM_OF_TRIES_TO_ACCESS_I2C_DEVICE  8

#ifdef CONFIG_I96XXX_M0
	#include "I96100.h"
	#include "clock_control_i96xxx_m0_api.h"
#elif defined(CONFIG_I94XXX)
	#include "I94100.h"
	#include "clock_control_i94xxx_api.h"
#endif

#define MAX_REG_ADDR_SIZE   4

static uint8_t dummy_msg;
static size_t status_debug;

static void transmit_byte(I2C_T *i2c, struct i2c_i9xxxx_cfg_t *cfg_hndl,
		struct i2c_i9xxxx_runtime_t *runtime_handle)
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
		if (I2C_I9XXXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
		{
			I2C_SET_DATA(i2c, 0x00);
		}
	}
	if (I2C_I9XXXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
	{
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
	}
}


static void transmit_reg_addr_byte(I2C_T *i2c,
		struct i2c_i9xxxx_cfg_t *cfg_hndl,
		struct i2c_i9xxxx_runtime_t *runtime_handle)
{
	uint8_t  const *reg_addr_arr;

	reg_addr_arr = runtime_handle->reg_addr_arr;
	I2C_SET_DATA(i2c, *reg_addr_arr++);
	runtime_handle->reg_addr_arr = reg_addr_arr;
	runtime_handle->reg_addr_left_to_transmit--;
}


static void slave_end_of_transmition(struct i2c_i9xxxx_cfg_t *cfg_hndl,
		struct i2c_i9xxxx_runtime_t *runtime_handle)
{
	size_t   transmitted_data_size;

	transmitted_data_size = runtime_handle->transmitted_data_size;

	if (transmitted_data_size)
	{
		struct dev_desc_t *callback_tx_dev;

		callback_tx_dev = cfg_hndl->callback_tx_dev;
		if ((0 == runtime_handle->tx_data_size) && (NULL != callback_tx_dev))
		{
			DEV_CALLBACK(callback_tx_dev ,
					CALLBACK_TX_DONE, (void*)transmitted_data_size);
			runtime_handle->transmitted_data_size = 0;
			runtime_handle->tx_data = NULL;
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
	struct i2c_i9xxxx_runtime_t *runtime_handle;
	struct i2c_i9xxxx_cfg_t *cfg_hndl;
	uint16_t curr_data_pos;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(i2c_i9xxxx, adev);
	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(i2c_i9xxxx, adev);

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
		if (I2C_I9XXXX_RCV_DATA_SIZE_BUFFER > curr_data_pos)
		{
			uint8_t data;

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
		slave_end_of_transmition(cfg_hndl, runtime_handle);
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
			DEV_CALLBACK(callback_rx_dev,
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
 * function : I2C_MasterTX
 *
 */
static void I2C_MasterTX(struct i2c_i9xxxx_cfg_t *cfg_hndl,
		struct i2c_i9xxxx_runtime_t *runtime_handle,
		uint32_t u32Status, I2C_T *i2c)
{
	uint8_t  end_of_transmition;
	uint8_t  reg_addr_left_to_transmit;

	reg_addr_left_to_transmit = runtime_handle->reg_addr_left_to_transmit;

	end_of_transmition = 0;
	switch(u32Status)
	{
	case 0x08:
		/* START has been transmitted and Write SLA+W to Register I2CDAT. */
		I2C_SET_DATA(i2c, runtime_handle->remote_slave_addr << 1);
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		runtime_handle->device_access_tries--;
		break;

	case 0x18:
		/* SLA+W has been transmitted and ACK has been received. */
		if (reg_addr_left_to_transmit)
		{
			transmit_reg_addr_byte(i2c, cfg_hndl, runtime_handle);
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else if (runtime_handle->tx_data_size)
		{
			transmit_byte(i2c, cfg_hndl, runtime_handle);
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else
		{
			end_of_transmition = 1;
		}
		break;

	case 0x20:
		/* SLA+W has been transmitted and NACK has been received. */
		if (runtime_handle->device_access_tries)
		{
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO | I2C_CTL_STA | I2C_CTL_SI);
		}
		else
		{
			runtime_handle->i2c_error = I2C_DEVICE_TIMEOUT;
			end_of_transmition = 1;
		}
		break;

	case 0x28:
		/* DATA has been transmitted and ACK has been received. */
		if (reg_addr_left_to_transmit)
		{
			transmit_reg_addr_byte( i2c, cfg_hndl, runtime_handle);
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else if (runtime_handle->tx_data_size)
		{
			transmit_byte(i2c, cfg_hndl, runtime_handle);
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else
		{
			end_of_transmition = 1;
		}
		break;

	case 0x30:
		/* DATA has been transmitted and NACK has been received. */
		runtime_handle->i2c_error = I2C_DATA_NAK_ERROR;
		end_of_transmition = 1;
		break;

	case 0x38:
		/* Arbitration Lost */
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		runtime_handle->i2c_error = I2C_ARBITRATION_ERROR;
		end_of_transmition = 1;
		break;

	case 0x00:
		/* Bus Error */
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		runtime_handle->i2c_error = I2C_BUS_ERROR;
		end_of_transmition = 1;
		break;

	default:
		status_debug = u32Status;
		CRITICAL_ERROR("unimplemented status\n");
		break;
	}

	if (end_of_transmition)
	{
		//NOTE: Must declare STOP before disabling interrupt due to
		//      unknown bug that will randomly make the I2C bus freeze at
		//      I2C_GET_STATUS(i2c_regs) = 0x00000000
		I2C_STOP(i2c);// SI bit is cleared inside function call
		I2C_DisableInt(i2c);
		runtime_handle->tx_data = NULL;
		os_queue_send_without_wait(
				runtime_handle->WaitQueue, (void *) &dummy_msg);
	}
}


/*
 * function : I2C_MasterRX
 *
 */
static void I2C_MasterRX(struct i2c_i9xxxx_cfg_t *cfg_hndl,
		struct i2c_i9xxxx_runtime_t *runtime_handle,
		uint32_t u32Status, I2C_T *i2c)
{
	uint8_t data;
	uint8_t  *rx_data;
	uint16_t curr_data_pos;
	uint8_t  reg_addr_left_to_transmit;
	uint8_t end_of_transmition;

	curr_data_pos = runtime_handle->curr_data_pos;
	reg_addr_left_to_transmit = runtime_handle->reg_addr_left_to_transmit;
	rx_data = runtime_handle->rx_data;


	end_of_transmition = 0;
	switch(u32Status)
	{
	case 0x08:
		if (reg_addr_left_to_transmit)
		{
			/* START has been transmitted and Write SLA+W to Register I2CDAT. */
			I2C_SET_DATA(i2c, (runtime_handle->remote_slave_addr << 1));
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else
		{
			/* Repeat START has been transmitted and prepare SLA+R */
			I2C_SET_DATA(i2c, (runtime_handle->remote_slave_addr << 1) | 0x01);
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
		}
		runtime_handle->device_access_tries--;
		break;

	case 0x18:
		/* SLA+W has been transmitted and ACK has been received. */
		if (reg_addr_left_to_transmit)
		{
			transmit_reg_addr_byte(i2c, cfg_hndl, runtime_handle);
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else
		{
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_STA_SI);
		}
		break;

	case 0x20:
		/* SLA+W has been transmitted and NACK has been received. */
		if (runtime_handle->device_access_tries)
		{
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO | I2C_CTL_STA | I2C_CTL_SI);
		}
		else
		{
			runtime_handle->i2c_error = I2C_DEVICE_TIMEOUT;
			end_of_transmition = 1;
		}
		break;

	case 0x28:
		/* DATA has been transmitted and ACK has been received. */
		if (reg_addr_left_to_transmit)
		{
			transmit_reg_addr_byte(i2c, cfg_hndl, runtime_handle);
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else
		{
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_STA_SI);
		}
		break;

	case 0x10:
		/* Repeat START has been transmitted and prepare SLA+R */
		I2C_SET_DATA(i2c, (runtime_handle->remote_slave_addr << 1) | 0x01);
		I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
		runtime_handle->device_access_tries--;
		break;

	case 0x40:
		/* SLA+R has been transmitted and ACK has been received. */
		if ((curr_data_pos + 1) == runtime_handle->size_of_data_to_receive)
		{
			// last byte
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else
		{
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
		}
		break;

	case 0x48:
		end_of_transmition = 1;
		break;

	case 0x50:
		/* DATA has been received and NACK has been returned */
		data = I2C_GET_DATA(i2c);
		rx_data[curr_data_pos] = data;
		curr_data_pos++;
		runtime_handle->curr_data_pos = curr_data_pos;
		if ((curr_data_pos + 1) == runtime_handle->size_of_data_to_receive)
		{
			// last byte
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI);
		}
		else
		{
			I2C_SET_CONTROL_REG(i2c, I2C_CTL_SI_AA);
		}
		break;

	case 0x58:
		data = I2C_GET_DATA(i2c);
		rx_data[curr_data_pos] = data;
		curr_data_pos++;
		runtime_handle->curr_data_pos = curr_data_pos;
		if (curr_data_pos == runtime_handle->size_of_data_to_receive)
		{
			end_of_transmition = 1;
		}
		else
		{
			CRITICAL_ERROR("unexpected status\n");
		}
		break;

	case 0x30:
		/* DATA has been transmitted and NACK has been received. */
		runtime_handle->i2c_error = I2C_DATA_NAK_ERROR;
		end_of_transmition = 1;
		break;

	case 0x38:
		/* Arbitration Lost */
		runtime_handle->i2c_error = I2C_ARBITRATION_ERROR;
		end_of_transmition = 1;
		break;

	case 0x00:
		/* Bus Error */
		runtime_handle->i2c_error = I2C_BUS_ERROR;
		end_of_transmition = 1;
		break;

	default:
		status_debug = u32Status;
		CRITICAL_ERROR("unimplemented status\n");
		break;
	}

	if (end_of_transmition)
	{
		//NOTE: Must declare STOP before disabling interrupt due to
		//      unknown bug that will randomly make the I2C bus freeze at
		//      I2C_GET_STATUS(i2c_regs) = 0x00000000
		I2C_STOP(i2c);// SI bit is cleared inside function call
		I2C_DisableInt(i2c);
		runtime_handle->rx_data = NULL;
		os_queue_send_without_wait(
				runtime_handle->WaitQueue, (void *) &dummy_msg);
	}
}


static uint8_t i2c_i9xxxx_callback(struct dev_desc_t *adev ,
		uint8_t aCallback_num , void * aCallback_param1,
		void * aCallback_param2)
{
	struct i2c_i9xxxx_runtime_t *runtime_handle;
	struct i2c_i9xxxx_cfg_t *cfg_hndl;
	uint32_t u32Status;
	I2C_T *i2c_regs;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(i2c_i9xxxx, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(i2c_i9xxxx, adev);
	i2c_regs =(I2C_T *)cfg_hndl->base_address;


	u32Status = I2C_GET_STATUS(i2c_regs);

	if(I2C_GET_TIMEOUT_FLAG(i2c_regs))
	{
		/* Clear I2C0 Timeout Flag */
		I2C_ClearTimeoutFlag(i2c_regs);
		CRITICAL_ERROR("TO DO\n");
	}
	else
	{
		if (I2C_I9XXXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
		{
			I2C_SlaveTRx(u32Status, i2c_regs, adev);
		}
		else
		{
			if (NULL != runtime_handle->rx_data)
			{
				I2C_MasterRX(cfg_hndl, runtime_handle, u32Status, i2c_regs);
			}
			else if (NULL != runtime_handle->tx_data)
			{
				I2C_MasterTX(cfg_hndl, runtime_handle, u32Status, i2c_regs);
			}
		}
	}

	return 0;
}


/**
 * i2c_i9xxxx_pwrite()
 *
 * return:
 */
static size_t i2c_i9xxxx_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct i2c_i9xxxx_cfg_t *cfg_hndl;
	struct i2c_i9xxxx_runtime_t *runtime_handle;
	size_t   tx_data_size;
	size_t   transmitted_data_size;
	uint8_t  const *tx_data;
	I2C_T *i2c_regs;

	if (1 >= aLength)
	{
		return 0;
	}

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(i2c_i9xxxx, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(i2c_i9xxxx, adev);

	i2c_regs =(I2C_T *)cfg_hndl->base_address;

	if (runtime_handle->tx_data_size)
	{
		CRITICAL_ERROR("still running\n");
	}

	while (0xf8 != I2C_GET_STATUS(i2c_regs)); //wait till bus is not released

	if (I2C_I9XXXX_API_MASTER_MODE == cfg_hndl->master_slave_mode)
	{
		// first byte is address of slave
		tx_data_size = aLength - 1;
		transmitted_data_size = 1;
		tx_data = &apData[1];
		runtime_handle->remote_slave_addr = apData[0];
		I2C_START(i2c_regs);
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


	return 0;
}


/**
 * master_write()
 *
 * return:
 */
static uint8_t master_write(struct i2c_i9xxxx_cfg_t *cfg_hndl,
				struct i2c_i9xxxx_runtime_t *runtime_handle,
				struct i2c_api_master_write_t *wr_struct)
{
	I2C_T *i2c_regs;
	os_mutex_t  mutex;
	uint8_t  reg_addr_arr[MAX_REG_ADDR_SIZE];
	size_t   reg_addr_size;
	size_t   reg_addr;
	size_t   num_of_bytes_to_write;

	wr_struct->num_of_bytes_written = 0;
	num_of_bytes_to_write = wr_struct->num_of_bytes_to_write;

	if (0 == num_of_bytes_to_write) return 0;

	reg_addr_size = wr_struct->reg_addr_size;
	if (MAX_REG_ADDR_SIZE < reg_addr_size)
	{
		wr_struct->i2c_error = I2C_BAD_REG_ADDR_SIZE;
		return 1;
	}

	mutex = runtime_handle->mutex;
	os_mutex_take_infinite_wait(mutex);
	if (0 == runtime_handle->init_done) // check if driver still running
	{
		wr_struct->i2c_error = I2C_NOT_INITIALIZED;
		os_mutex_give(mutex);
		return 1;
	}

	i2c_regs =(I2C_T *)cfg_hndl->base_address;
	while (0xf8 != I2C_GET_STATUS(i2c_regs)); //wait till bus is not released

	runtime_handle->device_access_tries = NUM_OF_TRIES_TO_ACCESS_I2C_DEVICE;
	runtime_handle->tx_data = wr_struct->tx_data;
	runtime_handle->tx_data_size = num_of_bytes_to_write;
	runtime_handle->transmitted_data_size = 0;
	runtime_handle->i2c_error = I2C_OK;

	runtime_handle->remote_slave_addr = wr_struct->device_addr_7bit;
	reg_addr = wr_struct->reg_addr;
	runtime_handle->reg_addr_left_to_transmit = reg_addr_size;
	while(reg_addr_size)
	{
		reg_addr_arr[reg_addr_size - 1] = reg_addr & 0xff;
		reg_addr = reg_addr >> 8;
		reg_addr_size--;
	}
	runtime_handle->reg_addr_arr = reg_addr_arr;

	I2C_EnableInt(i2c_regs);
	I2C_START(i2c_regs);

	os_queue_receive_infinite_wait(runtime_handle->WaitQueue,  &dummy_msg);
	wr_struct->i2c_error = runtime_handle->i2c_error;

	if (0 == runtime_handle->device_access_tries)
	{
		wr_struct->num_of_bytes_written = 0;
	}
	else
	{
		wr_struct->num_of_bytes_written =
					runtime_handle->transmitted_data_size;
	}

	os_mutex_give(mutex);
	return 0;
}


static uint8_t master_read(struct i2c_i9xxxx_cfg_t *cfg_hndl,
				struct i2c_i9xxxx_runtime_t *runtime_handle,
				struct i2c_api_master_read_t *rd_struct)
{
	os_mutex_t  mutex;
	I2C_T *i2c_regs;
	uint8_t  reg_addr_arr[MAX_REG_ADDR_SIZE];
	size_t   num_of_bytes_to_read;
	size_t   num_of_bytes_that_was_read;
	size_t   reg_addr_size;
	size_t   reg_addr;

	rd_struct->num_of_bytes_that_was_read = 0;
	num_of_bytes_to_read = rd_struct->num_of_bytes_to_read;
	if (1 > num_of_bytes_to_read) return 0;

	reg_addr_size = rd_struct->reg_addr_size;
	if (MAX_REG_ADDR_SIZE < reg_addr_size)
	{
		rd_struct->i2c_error = I2C_BAD_REG_ADDR_SIZE;
		return 1;
	}

	if (I2C_I9XXXX_API_MASTER_MODE != cfg_hndl->master_slave_mode) return 1;

	mutex = runtime_handle->mutex;
	os_mutex_take_infinite_wait(mutex);
	if (0 == runtime_handle->init_done) // check if driver still running
	{
		os_mutex_give(mutex);
		rd_struct->i2c_error = I2C_NOT_INITIALIZED;
		return 1;
	}

	i2c_regs =(I2C_T *)cfg_hndl->base_address;
	while (0xf8 != I2C_GET_STATUS(i2c_regs)); //wait till bus is not released

	// first byte is address of slave
	runtime_handle->device_access_tries = NUM_OF_TRIES_TO_ACCESS_I2C_DEVICE;
	runtime_handle->i2c_error = I2C_OK;
	runtime_handle->curr_data_pos = 0;
	runtime_handle->size_of_data_to_receive = num_of_bytes_to_read;
	runtime_handle->rx_data = rd_struct->rx_data;
	runtime_handle->remote_slave_addr = rd_struct->device_addr_7bit;
	reg_addr = rd_struct->reg_addr;
	runtime_handle->reg_addr_left_to_transmit = reg_addr_size;
	while(reg_addr_size)
	{
		reg_addr_arr[reg_addr_size - 1] = reg_addr & 0xff;
		reg_addr = reg_addr >> 8;
		reg_addr_size--;
	}
	runtime_handle->reg_addr_arr = reg_addr_arr;

	I2C_EnableInt(i2c_regs);
	I2C_START(i2c_regs);

	os_queue_receive_infinite_wait(runtime_handle->WaitQueue,  &dummy_msg);

	rd_struct->i2c_error = runtime_handle->i2c_error;

	if (0 == runtime_handle->device_access_tries)
	{
		num_of_bytes_that_was_read = 0;
	}
	else
	{
		num_of_bytes_that_was_read = runtime_handle->curr_data_pos;
	}
	rd_struct->num_of_bytes_that_was_read = num_of_bytes_that_was_read;
	os_mutex_give(mutex);
	return 0;
}



static uint8_t  device_start(struct dev_desc_t *adev,
		struct i2c_i9xxxx_cfg_t *cfg_hndl,
		struct i2c_i9xxxx_runtime_t *runtime_handle)
{
	I2C_T *i2c_regs;
	int i2c_irq;
	uint32_t i2c_module_rst;
	struct dev_desc_t	*i2c_clk_dev;

	if (runtime_handle->init_done) return 0;

	i2c_regs = (I2C_T *)cfg_hndl->base_address;

	runtime_handle->mutex = os_create_mutex();
	runtime_handle->WaitQueue = os_create_queue(1, sizeof(uint8_t ));
	if(NULL == runtime_handle->WaitQueue)
	{
		CRITICAL_ERROR("Wait Queue NULL");
	}

	pin_control_api_set_pin_function(cfg_hndl->SCL_pinout);
	pin_control_api_enable_input_schmitt_trigger(cfg_hndl->SCL_pinout);
	pin_control_api_set_pin_function(cfg_hndl->SDA_pinout);
	pin_control_api_enable_input_schmitt_trigger(cfg_hndl->SDA_pinout);

	if(I2C1 == i2c_regs)
	{
#ifdef CONFIG_I96XXX_M0
		CRITICAL_ERROR("not implemented yet");
#endif
		i2c_irq = I2C1_IRQn;
		i2c_module_rst = I2C1_RST;
		i2c_clk_dev = i9xxxx_i2c1_clk_dev;
	}
	else if(I2C0 == i2c_regs)
	{
		i2c_irq = I2C0_IRQn;
		i2c_module_rst = I2C0_RST;
		i2c_clk_dev = i9xxxx_i2c0_clk_dev;
	}
	else
	{
		CRITICAL_ERROR("I2C base has not been set");
	}


	DEV_IOCTL(i2c_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL(i2c_clk_dev, CLK_IOCTL_ENABLE);

	/* Reset I2C module */
	SYS_ResetModule(i2c_module_rst);

	/* Configure I2C and set I2C baud rate */
	I2C_Open(i2c_regs, cfg_hndl->baud_rate);

	if (I2C_I9XXXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
	{
		runtime_handle->slave_address = cfg_hndl->slave_address;
		I2C_SetSlaveAddr(i2c_regs, 0, (cfg_hndl->slave_address) >> 1 , 0);
		I2C_SetSlaveAddrMask(i2c_regs, 0, 0x01);

		/* I2C enter no address SLV mode */
		I2C_SET_CONTROL_REG(i2c_regs, I2C_CTL_SI_AA);
	}

	I2C_EnableInt(i2c_regs);

	irq_register_device_on_interrupt(i2c_irq, adev);
	irq_set_priority(i2c_irq, INTERRUPT_PRIORITY_FOR_I2C );
	irq_enable_interrupt(i2c_irq);
	runtime_handle->init_done = 1;
	return 0;
}


static void SYS_put_on_reset(uint32_t u32ModuleIndex)
{
    /* Generate reset signal to the corresponding module */
    *(volatile uint32_t *)((uint32_t)&SYS->IPRST0 + (u32ModuleIndex >> 24)) |=
    										1 << (u32ModuleIndex & 0x00ffffff);
}


static uint8_t  set_new_slave_addr(
		struct i2c_i9xxxx_cfg_t *cfg_hndl,
		struct i2c_i9xxxx_runtime_t *runtime_handle,
		uint8_t *new_slave_addr_7bit)
{
	I2C_T *i2c_regs;

	if (I2C_I9XXXX_API_SLAVE_MODE == cfg_hndl->master_slave_mode)
	{
		i2c_regs = (I2C_T *)cfg_hndl->base_address;
		runtime_handle->slave_address = *new_slave_addr_7bit;

		I2C_SetSlaveAddr(i2c_regs, 0, *new_slave_addr_7bit >> 1 , 0);
		return 0;
	}
	return 1;
}


static uint8_t  device_stop(
		struct i2c_i9xxxx_cfg_t *cfg_hndl,
		struct i2c_i9xxxx_runtime_t *runtime_handle)
{
	os_mutex_t  mutex;
	I2C_T *i2c_regs;
	int i2c_irq;
	uint32_t i2c_module_rst;
	struct dev_desc_t	*i2c_clk_dev;

	mutex = runtime_handle->mutex;
	os_mutex_take_infinite_wait(mutex);
	if (0 == runtime_handle->init_done) // check if driver still running
	{
		os_mutex_give(mutex);
		return 1;
	}

	runtime_handle->init_done = 0;

	i2c_regs = (I2C_T *)cfg_hndl->base_address;
	if(I2C0 == i2c_regs)
	{
		i2c_irq = I2C0_IRQn;
		i2c_module_rst = I2C0_RST;
		i2c_clk_dev = i9xxxx_i2c0_clk_dev;
	}
	else
	{
		i2c_irq = I2C1_IRQn;
		i2c_module_rst = I2C1_RST;
		i2c_clk_dev = i9xxxx_i2c1_clk_dev;
	}

	irq_disable_interrupt(i2c_irq);
	I2C_DisableInt(i2c_regs);
	SYS_put_on_reset(i2c_module_rst);

	pin_control_api_clear_pin_function(cfg_hndl->SCL_pinout);
	pin_control_api_clear_pin_function(cfg_hndl->SDA_pinout);
	DEV_IOCTL(i2c_clk_dev, CLK_IOCTL_DISABLE);
	os_queue_delete(runtime_handle->WaitQueue);
	os_mutex_give(mutex);
	os_delay_ms(5); // wait for all tasks to stop waiting on mutex
	os_delete_mutex(mutex);
	return 0;
}


/**
 * i2c_i9xxxx_ioctl()
 *
 * return:
 */
static uint8_t i2c_i9xxxx_ioctl( struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct i2c_i9xxxx_cfg_t *cfg_hndl;
	struct i2c_i9xxxx_runtime_t *runtime_handle;
	I2C_T *i2c_regs;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(i2c_i9xxxx, adev);
	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		return 1;
	}

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(i2c_i9xxxx, adev);
	i2c_regs = (I2C_T *)cfg_hndl->base_address;
	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		return device_start(adev, cfg_hndl, runtime_handle);
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
		break;
	case IOCTL_I2C_MASTER_READ :
		return master_read(cfg_hndl, runtime_handle, aIoctl_param1);
	case IOCTL_I2C_MASTER_WRITE :
		return master_write(cfg_hndl, runtime_handle, aIoctl_param1);
	case IOCTL_DEVICE_STOP :
		return device_stop(cfg_hndl, runtime_handle);
	case IOCTL_I2C_SET_NEW_SLAVE_ADDR:
		return set_new_slave_addr(cfg_hndl, runtime_handle, aIoctl_param1);
	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                     i2c_i9xxxx
#define MODULE_IOCTL_FUNCTION           i2c_i9xxxx_ioctl
#define MODULE_CALLBACK_FUNCTION        i2c_i9xxxx_callback
#define MODULE_PWRITE_FUNCTION          i2c_i9xxxx_pwrite
#include "add_module.h"
