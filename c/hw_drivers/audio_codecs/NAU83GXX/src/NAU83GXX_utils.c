/*
 *  NAU83GXX_utils.c
 */

#ifndef FOR_EXPORT
	#include "_project.h"
#else
	#include "hal.h"
#endif
#include "dev_management_api.h"

#include "os_wrapper.h"

//#define DEBUG
#include "PRINTF_api.h"

#include "NAU83GXX_api.h"
#include "NAU83GXX.h"
#ifndef FOR_EXPORT
	#include "i2c_api.h"
#endif

#include "NAU83GXX_G10.c"
#include "NAU83GXX_G20.c"
#include "NAU83GXX_G60.c"


uint8_t nau83gxx_read(struct dev_desc_t *i2c_dev, uint8_t device_addr,
				uint16_t reg_addr, uint8_t *read_data, uint16_t size_to_read)
{
	uint8_t retries = 3;
	struct i2c_api_master_read_t  i2c_read_struct;

	i2c_read_struct.device_addr_7bit = device_addr;
	i2c_read_struct.num_of_bytes_to_read = size_to_read;
	i2c_read_struct.reg_addr_size = 2;
	i2c_read_struct.reg_addr = reg_addr;
	i2c_read_struct.rx_data = read_data;

	i2c_read_struct.i2c_error = 1;
	while((i2c_read_struct.i2c_error) && (retries--))
	{
		DEV_IOCTL(i2c_dev, IOCTL_I2C_MASTER_READ, &i2c_read_struct);
	}
	return (i2c_read_struct.i2c_error ? NAU83GXX_RC_I2C_ERROR : NAU83GXX_RC_OK);
}

uint8_t nau83gxx_read_wordU16(struct dev_desc_t *i2c_dev, uint8_t device_addr,
				uint16_t reg_addr, uint16_t *read_dat)
{
	uint8_t rc;
	uint8_t read_i2c_data[2];

	rc = nau83gxx_read(i2c_dev, device_addr, reg_addr, read_i2c_data, 2);
	*read_dat = read_i2c_data[1];
	*read_dat = *read_dat | (read_i2c_data[0] << 8);
	return rc;
}


uint8_t nau83gxx_write(struct dev_desc_t *i2c_dev, uint8_t device_addr,
		uint16_t reg_addr, const uint8_t *data, size_t write_data_size)
{
	struct i2c_api_master_write_t  i2c_write_struct;
	int retries;

	if(NULL == i2c_dev)
	{
		CRITICAL_ERROR("I2C Device not specified");
	}

	i2c_write_struct.device_addr_7bit = device_addr;
	i2c_write_struct.num_of_bytes_to_write = write_data_size;
	i2c_write_struct.tx_data = data;
	i2c_write_struct.reg_addr = reg_addr;
	i2c_write_struct.reg_addr_size = 2;
	i2c_write_struct.num_of_bytes_written = 0;

	PRINTF_DBG( "sending reg = 0x%02x : 0x%02x 0x%02x 0x%02x 0x%02x\r\n",
			i2c_write_struct.reg_addr, i2c_write_struct.tx_data[0],
			i2c_write_struct.tx_data[1], i2c_write_struct.tx_data[2],
										i2c_write_struct.tx_data[3] );
	i2c_write_struct.i2c_error = 1;
	retries = 3;
	while (i2c_write_struct.i2c_error)
	{
		if (0 == retries--)
		{
			PRINTF_DBG("error: 0x%02X\r\n",i2c_write_struct.i2c_error);
			return NAU83GXX_RC_I2C_ERROR;
		}

		PRINTF_DBG("error: 0x%02X\tRetrying...\r\n",i2c_write_struct.i2c_error);
		DEV_IOCTL(i2c_dev, IOCTL_I2C_MASTER_WRITE, &i2c_write_struct);
	}

	return NAU83GXX_RC_OK;
}


uint8_t nau83gxx_write_wordU16(struct dev_desc_t *i2c_dev,
		uint8_t device_addr, uint16_t reg_addr, uint16_t write_dat)
{
	uint8_t write_i2c_data[2];

	write_i2c_data[1] = write_dat & 0xFF;
	write_i2c_data[0] = (write_dat >> 8) & 0xFF;
	return nau83gxx_write(i2c_dev, device_addr, reg_addr, write_i2c_data, 2);
}


void NAU83GXX_OCP_recovery(struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	switch(runtime_handle->chip_type)
	{
	case NAU83GXX_CHIP_TYPE_G10:
	case NAU83GXX_CHIP_TYPE_G20:
		run_G10_G20_OCP_recovery(config_handle, runtime_handle);
		break;
	case NAU83GXX_CHIP_TYPE_G60:
		run_G60_OCP_recovery(config_handle, runtime_handle);
		break;
	default:
		break;
	}
}


uint8_t  NAU83GXX_send_register_array(
			struct dev_desc_t *i2c_dev, uint8_t device_addr,
			const struct NAU83GXX_reg_s *reg_array, uint16_t array_size)
{
	uint16_t i;
	uint8_t ret;

	for(i = 0; i < array_size; i++)
	{
		if(0xDDDD == reg_array[i].u8Reg)
		{
			os_delay_ms(
					reg_array[i].u8Value[1] + (reg_array[i].u8Value[0] << 8));
		}
		else
		{
			ret = nau83gxx_write(i2c_dev, device_addr,
					reg_array[i].u8Reg, reg_array[i].u8Value, 2);
			if (ret) return ret;
		}
	}
	return NAU83GXX_RC_OK;
}


uint8_t NAU83GXX_update_real_device_id(struct NAU83GXX_config_t *config_handle,
		struct dev_desc_t *i2c_dev, struct NAU83GXX_runtime_t *runtime_handle)
{
	uint8_t rc;
	uint8_t read_i2c_data[2];
	uint8_t device_id;
	uint8_t device_addr;
	uint8_t i;

	for (i = 0; i < config_handle->i2c_addr_arr_size; i++)
	{
		device_addr = config_handle->i2c_addr_arr[i];
		rc = nau83gxx_read(
				i2c_dev, device_addr, NAU83GXX_REG_DEVICE_ID, read_i2c_data, 2);
		if (0 == rc)
		{
			runtime_handle->dev_addr = device_addr;
			device_id = read_i2c_data[1] & 0xF0;
			runtime_handle->chip_type = device_id;
			return 0;
		}
	}

	return NAU83GXX_RC_DEVICE_DOES_NOT_EXIST;
}


uint8_t nau83gxx_init_i2c_regs(struct NAU83GXX_config_t *config_handle,
								struct NAU83GXX_runtime_t *runtime_handle)
{
	//delay to make sure chip was powered up
	os_delay_ms(5);

	switch(runtime_handle->chip_type)
	{
	case NAU83GXX_CHIP_TYPE_G10:
		return init_G10_regs(config_handle, runtime_handle);
	case NAU83GXX_CHIP_TYPE_G20:
		return init_G20_regs(config_handle, runtime_handle);
	case NAU83GXX_CHIP_TYPE_G60:
		return init_G60_regs(config_handle, runtime_handle);
	default:
		return NAU83GXX_RC_NOT_SUPPORTED_DEVICE_ID;
		break;
	}
}
