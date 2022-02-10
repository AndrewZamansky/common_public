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

/*
 * Array table to be flashed onto 83G10. This is device specific and will not
 * work for any other device.
 *
 */

// Command for 83G10(transfer via I2C0)
static const struct NAU83GXX_reg_s cmd_83G10b_init[] = {
		//-- 2-Byte Address -- 2-Byte Data -----//
//		{  0x0000  ,  {  0x00  ,  0x01  }  }  ,  // Reset all registers
//		{  0x0000  ,  {  0x00  ,  0x00  }  }  ,  // Release reset
		{  0x0002  ,  {  0x00  ,  0x01  }  }  ,  // Latch I2C address on GPIO's
		{  0x0003  ,  {  0x00  ,  0x04  }  }  ,  // Block MCLK
		{  0x000E  ,  {  0x00  ,  0x00  }  }  ,  // Default I2S setup
		//-------
//		{  0x000C  ,  {  0x0c  ,  0x24  }  }  ,  // ADCOUT channels
		{  0x0014  ,  {  0x00  ,  0x63  }  }  ,  // Set I-sense digital gain to +1.25dB
		{  0x0076  ,  {  0x40  ,  0x00  }  }  ,  // Set I-sense PGA output stage Bias
		{  0x0077  ,  {  0x07  ,  0x80  }  }  ,  // Set I-sense PGA Bias Current to minimum
		{  0x007F  ,  {  0x00  ,  0x30  }  }  ,  // Set I-sense PGA to +16dB gain
		{  0x0028  ,  {  0x80  ,  0x02  }  }  ,  // Set normal I2S with 128x ADC OSR
		{  0x0029  ,  {  0x00  ,  0x80  }  }  ,  // Set normal 128x DAC OSR
		{  0x0007  ,  {  0x00  ,  0xb5  }  }  ,  // Enable SAR ADC with 4usec sample time, 70kOhm and 1 usec compare cycle
		{  0x002C  ,  {  0x20  ,  0xE0  }  }  ,  // Set ALC gain limits
		{  0x002D  ,  {  0x53  ,  0x50  }  }  ,  // Set ALC settings
		{  0x002E  ,  {  0x33  ,  0x10  }  }  ,  // Enable Auto-Attenuator, Enable ALC & set threshold & mode
		{  0x002F  ,  {  0x01  ,  0x3F  }  }  ,  // was 0x00BF changed after luxare project tests
		{  0x0040  ,  {  0x08  ,  0x01  }  }  ,  // Set target sample rate
		{  0x0004  ,  {  0x00  ,  0x07  }  }  ,  // Enable DAC & ADC's (??)
//		{  0x0017  ,  {  0x3a  ,  0x02  }  }  ,  // Maximum Boost Voltage and Margin
		{  0x0030  ,  {  0x7d  ,  0x24  }  }  ,  // ?? Unknown, according to trim
		{  0x0033  ,  {  0xbd  ,  0xc0  }  }  ,  // ?? Unknown, according to trim
		{  0x0018  ,  {  0xc7  ,  0xf8  }  }  ,  // Set Boost Attack/Decay & Hold Time and TC EN
		{  0x0073  ,  {  0x00  ,  0x04  }  }  ,  // Set DAC reference
		{  0x0063  ,  {  0x00  ,  0x30  }  }  ,  // Set DAC reference capacitors
		{  0x0064  ,  {  0x40  ,  0x00  }  }  ,  // Set Auto-Attenuator minimum gain
		{  0x0065  ,  {  0x40  ,  0x88  }  }  ,  // Boost Settings 1
		{  0x0066  ,  {  0x0A  ,  0xD4  }  }  ,  // Boost Settings 2
//		{  0x006B  ,  {  0x78  ,  0xF0  }  }  ,  // Peak Current 4A, Low Voltage 2.8V
		{  0x006C  ,  {  0x00  ,  0x0F  }  }  ,  // What is this?
		{  0x0060  ,  {  0x00  ,  0x30  }  }  ,  // VMID resistor selection
		{  0x0001  ,  {  0x00  ,  0x01  }  }  ,  // Soft Reset
		{  0x0001  ,  {  0x00  ,  0x01  }  }  ,
		{  0x0061  ,  {  0x55  ,  0x55  }  }  ,  // Enable Bias, DAC & ADC's with clock gating
		{  0x0062  ,  {  0x00  ,  0x15  }  }  ,  // Enable Boost & Class-D & disable fast reference power up with clock gating
		{  0x0003  ,  {  0x08  ,  0xE0  }  }  ,  // Set ADC to 12kHz
		{  0x0068  ,  {  0x00  ,  0x0F  }  }  ,
		{  0x0004  ,  {  0x00  ,  0x67  }  }  ,
		{  0x001a  ,  {  0x00  ,  0x20  }  }  ,  // Disable DSP bypass -  Enable= 0010
#if BYTES_PER_PCM_CHANNEL == 2
		{  0x000D  ,  {  0x00  ,  0x02  }  }  ,  // I2S Port Word Length = 16bit
#endif
//		{  0x0001  ,  {  0x00  ,  0x01  }  }  ,  // Soft Reset
//		{  0x0001  ,  {  0x00  ,  0x01  }  }
};

uint16_t cmd_83G10b_init_size = sizeof(cmd_83G10b_init);

static const struct NAU83GXX_reg_s cmd_83G10c_init[] = {
		//-- 2-Byte Address -- 2-Byte Data -----//
//		{  0x0000  ,  {  0x00  ,  0x01  }  }  , // Reset all registers
//		{  0x0000  ,  {  0x00  ,  0x00  }  }  , // Release reset
		{  0x0002  ,  {  0x00  ,  0x01  }  }  , // Latch I2C address on GPIO's
		{  0x0003  ,  {  0x00  ,  0x04  }  }  , // Block MCLK
		{  0x001A  ,  {  0x00  ,  0x10  }  }  , // Stall DSP
		{  0x000E  ,  {  0x00  ,  0x00  }  }  , // Default I2S setup
		//-------
//		{  0x000C  ,  {  0x0C  ,  0x24  }  }  , // ADCOUT channels
		{  0x0014  ,  {  0x01  ,  0x62  }  }  , // Set I-sense digital gain to +10.25dB
		{  0x0076  ,  {  0x40  ,  0x00  }  }  , // Set I-sense PGA output stage Bias
		{  0x0077  ,  {  0x07  ,  0x80  }  }  , // Set I-sense PGA Bias Current to minimum
		{  0x007F  ,  {  0x00  ,  0x30  }  }  , // Set I-sense PGA to +12dB gain
		{  0x0028  ,  {  0x80  ,  0x01  }  }  , // Set normal I2S with 64x ADC OSR
		{  0x0029  ,  {  0x00  ,  0x80  }  }  , // Set normal 64x DAC OSR
		{  0x0007  ,  {  0x00  ,  0xB5  }  }  , // Enable SAR ADC with 4usec sample time, 70kOhm and 1 usec compare cycle
		{  0x002C  ,  {  0x20  ,  0xE0  }  }  , // Set ALC gain limits
		{  0x002D  ,  {  0x53  ,  0x50  }  }  , // Set ALC settings
		{  0x002E  ,  {  0x33  ,  0x10  }  }  , // Enable Auto-Attenuator, Enable ALC & set threshold & mode
		{  0x002F  ,  {  0x01  ,  0x3F  }  }  ,  // was 0x00BF changed after luxare project tests
		{  0x0040  ,  {  0x08  ,  0x01  }  }  , // Set target sample rate
		{  0x0004  ,  {  0x00  ,  0x07  }  }  , // Enable DAC & ADC's
//		{  0x0017  ,  {  0x3A  ,  0x02  }  }  , // Set Boost Maximum and Margin
		{  0x0030  ,  {  0x7d  ,  0x00  }  }  ,
		{  0x0033  ,  {  0xbd  ,  0xc0  }  }  ,
		{  0x0018  ,  {  0x87  ,  0xF8  }  }  , // Set Boost Attack/Decay & Hold Time and TC EN
		{  0x0073  ,  {  0x00  ,  0x04  }  }  , // Set DAC reference
		{  0x0063  ,  {  0x00  ,  0x30  }  }  , // Set DAC reference capacitors
		{  0x0064  ,  {  0x40  ,  0x07  }  }  , // Set Auto-Attenuator minimum gain
		{  0x0065  ,  {  0x4D  ,  0x82  }  }  , // Boost Settings 1
		{  0x0066  ,  {  0x0E  ,  0x94  }  }  , // Boost Settings 2
//		{  0x006B  ,  {  0x78  ,  0xF0  }  }  , // Peak Current 4.0A, Low Voltage 2.8V
		{  0x006C  ,  {  0x00  ,  0x0F  }  }  ,
		{  0x0060  ,  {  0x00  ,  0x00  }  }  , // VMID resistor selection
		{  0x0068  ,  {  0x06  ,  0x40  }  }  , // VREF Bandgap buffer ON, I/V Sense Ref Buffer Setting -1.5dB
		{  0x0001  ,  {  0x00  ,  0x00  }  }  , // Soft Reset
		{  0x0001  ,  {  0x00  ,  0x00  }  }  , // Soft Reset
		{  0x0061  ,  {  0x55  ,  0x55  }  }  , // Enable Bias, DAC & ADC's with clock gating
		{  0x0062  ,  {  0x00  ,  0x15  }  }  , // Enable Boost & Class-D & disable fast reference power up with clock gating
		{  0x0003  ,  {  0x08  ,  0xE0  }  }  , // Set MCLK, ADC & DAC clock dividers
		{  0x0004  ,  {  0x00  ,  0x67  }  }  ,// Enable DAC, IVsense channels, DSP_CLK = MCLK * 8 = 12.288M * 8 = 98.304M
		{  0x0068  ,  {  0x06  ,  0x4F  }  }  , // Enable clock multiplier
#if BYTES_PER_PCM_CHANNEL == 2
		{  0x000D  ,  {  0x00  ,  0x02  }  }  , // I2S Port Word Length = 16bit
#endif
//		{  0x0001  ,  {  0x00  ,  0x00  }  }  , // Soft Reset
//		{  0x0001  ,  {  0x00  ,  0x00  }  }  , // Soft Reset
//		{  0xDDDD  ,  {  0  ,  63  }  }  , // Firmware Delay
//		{  0x001A  ,  {  0x00  ,  0x20  }  }
};

uint16_t cmd_83G10c_init_size = sizeof(cmd_83G10c_init);

static const struct NAU83GXX_reg_s cmd_83G20a_init[] = {
		//-- 2-Byte Address -- 2-Byte Data -----//
//		{  0x0000 , {  0x00  ,  0x01  }  }  ,// Reset all registers
//		{  0x0000 , {  0x00  ,  0x00  }  }  ,// Release reset
		{  0x0002 , {  0x00  ,  0x01  }  }  ,// Latch I2C address on GPIO's
		{  0x0003 , {  0x00  ,  0x04  }  }  ,// Block MCLK
		{  0x001A , {  0x00  ,  0x10  }  }  ,// Stall DSP
		{  0x000E , {  0x00  ,  0x00  }  }  ,// Default I2S setup
		//-------
//		{  0x000C , {  0x0c  ,  0x24  }  }  ,// ADCOUT channels
		{  0x0014 , {  0x01  ,  0x62  }  }  ,// Set I-sense digital gain to +12.25dB vgain = 1 (?)
		{  0x0076 , {  0x40  ,  0x00  }  }  ,// Set I-sense PGA output stage Bias
		{  0x0077 , {  0x07  ,  0x80  }  }  ,// Set I-sense PGA Bias Current to minimum
		{  0x007F , {  0x00  ,  0x30  }  }  ,// Set I-sense PGA to +12dB gain
		{  0x0028 , {  0x80  ,  0x01  }  }  ,// Set normal I2S with 64x ADC OSR
		{  0x0029 , {  0x00  ,  0x80  }  }  ,// Set normal 64x DAC OSR
		{  0x0007 , {  0x00  ,  0x95  }  }  ,// Enable SAR ADC with 4usec sample time, 70kOhm and 0.5 usec compare cycle
		{  0x002C , {  0x20  ,  0xE2  }  }  ,// Set ALC gain limits
		{  0x002D , {  0x53  ,  0x50  }  }  ,// Set ALC settings
		{  0x002E , {  0xC6  ,  0xE0  }  }  ,// Enable ALC & set threshold & mode
		{  0x002F , {  0x00  ,  0xFF  }  }  ,// Set segment threshold
		{  0x0040 , {  0x08  ,  0x01  }  }  ,// Set target sample rate
		{  0x0004 , {  0x00  ,  0x07  }  }  ,// Enable DAC & ADC's
		{  0x0030 , {  0x7d  ,  0x00  }  }  ,// Set Isense Temperature Coefficient
		{  0x0033 , {  0xbd  ,  0xc0  }  }  ,// Set Isense TC low pas filter
		{  0x0018 , {  0x80  ,  0x00  }  }  ,// Set TC EN
		{  0x0073 , {  0x00  ,  0x08  }  }  ,// Set DAC reference
		{  0x0063 , {  0x00  ,  0x30  }  }  ,// Set DAC reference capacitors
		{  0x0064 , {  0x0c  ,  0x1b  }  }  ,// Set slew rate
//		{  0x006B , {  0x00  ,  0x70  }  }  ,// Disable UVLOP comparator and set threshold
		{  0x0031 , {  0x00  ,  0x48  }  }  ,// Set UVLOP attack, release & hold time, disable UVLOP gain limiter
		{  0x0032 , {  0x00  ,  0x00  }  }  ,// Set UVLOP minimum gain to no change
		{  0x0060 , {  0x00  ,  0x00  }  }  ,// VMID resistor selection
		{  0x0068 , {  0x06  ,  0x40  }  }  ,// VREF Bandgap buffer ON, I/V Sense Ref Buffer Setting -1.5dB
		{  0x0001 , {  0x00  ,  0x00  }  }  ,// Soft Reset
		{  0x0001 , {  0x00  ,  0x00  }  }  ,// Soft Reset
#if BYTES_PER_PCM_CHANNEL == 2
		{  0x000D  ,  {  0x00  ,  0x02  }  }  , // I2S Port Word Length = 16bit
#endif
		{  0x0061 , {  0x55  ,  0x55  }  }  ,// Enable Bias, DAC & ADC's with clock gating
		{  0x0062 , {  0x00  ,  0x14  }  }  ,// Enable Class-D & disable fast reference power up with clock gating
		{  0x0003 , {  0x08  ,  0xE0  }  }  ,// Set MCLK, ADC & DAC clock dividers
		{  0x0068 , {  0x06  ,  0x4F  }  }  ,// VREF Bandgap buffer ON, I/V Sense Ref Buffer Setting -1.5dB
		{  0x0004 , {  0x00  ,  0x67  }  }  ,//  DSP_CLK = (MCLK/2)*16
//		{  0x0001 , {  0x00  ,  0x00  }  }  , // Soft Reset
//		{  0x0001 , {  0x00  ,  0x00  }  }  , // Soft Reset
//		{  0xDDDD , {  0  ,  63  }  }  , // Firmware Delay
//		{  0x001A , {  0x00  ,  0x20  }  }  //   enable DSP output
};

uint16_t cmd_83G20a_init_size = sizeof(cmd_83G20a_init);



static const struct NAU83GXX_reg_s cmd_83G60_init[] = {
		//-- 2-Byte Address -- 2-Byte Data -----//
		{  0x0000 , {  0x00  ,  0x01  }  }  ,
};

uint16_t cmd_83G60_init_size = sizeof(cmd_83G60_init);


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

	PRINTF_DBG( "sending reg = 0x%02x : 0x%02x 0x%02x 0x%02x 0x%02x\n\r",
			i2c_write_struct.reg_addr, i2c_write_struct.tx_data[0],
			i2c_write_struct.tx_data[1], i2c_write_struct.tx_data[2],
										i2c_write_struct.tx_data[3] );
	i2c_write_struct.i2c_error = 1;
	retries = 3;
	while (i2c_write_struct.i2c_error)
	{
		if (0 == retries--)
		{
			PRINTF_DBG("error: 0x%02X\n\r",i2c_write_struct.i2c_error);
			return NAU83GXX_RC_I2C_ERROR;
		}

		PRINTF_DBG("error: 0x%02X\tRetrying...\n\r",i2c_write_struct.i2c_error);
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


#define OCP_THRESHOLD 5
void NAU83GXX_OCP_recovery(struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	uint16_t reg_0x06_data;
	uint16_t reg_0x62_data;
	uint8_t dev_addr;
	struct dev_desc_t * i2c_dev;
	uint8_t ocp_counter;
	uint8_t tries = 3;
	uint8_t ocp_tests;

	dev_addr = runtime_handle->dev_addr;
	i2c_dev = config_handle->i2c_dev;

	nau83gxx_read_wordU16(i2c_dev, dev_addr, 0x06, &reg_0x06_data);
	if (0 == (reg_0x06_data & 0x10))
	{
		return;
	}

	while (tries--)
	{
		if (0 == tries)
		{
			CRITICAL_ERROR("NAU83GXX cannot recover");
			// SW reset: maybe should got to callback to application
			//__asm__ __volatile__("cpsid i\n");
			//SYS_ResetChip();
		}
		ocp_counter = 0;
		ocp_tests = 10;
		while (ocp_tests--)
		{
			nau83gxx_read_wordU16(i2c_dev, dev_addr, 0x06, &reg_0x06_data);
			if (0x10 & reg_0x06_data)
			{
				ocp_counter++;
				//clear IRQ
				nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x06, 0xff);
				os_delay_ms(100);
			}
			if (OCP_THRESHOLD <= ocp_counter)
			{
				nau83gxx_read_wordU16(i2c_dev, dev_addr, 0x62, &reg_0x62_data);
				reg_0x62_data &= ~(0x30); // clear 'class-D enable' bits
				nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x62, reg_0x62_data);
				os_delay_ms(1);
				reg_0x62_data |= 0x10; // enable class-D
				nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x62, reg_0x62_data);
				os_delay_ms(10);
			}
		}
	}
}


 uint8_t  send_register_array(struct dev_desc_t *i2c_dev, uint8_t device_addr,
		 const struct NAU83GXX_reg_s *reg_array, uint16_t array_size)
{
	uint16_t i;
	uint8_t ret;

	for(i = 0; i < array_size; i++)
	{
		if(0xDDDD == reg_array[i].u8Reg)
		{
			os_delay_ms(reg_array[i].u8Value[1]);
		}
		else
		{
			ret = nau83gxx_write(i2c_dev, device_addr,
					reg_array[i].u8Reg, reg_array[i].u8Value, 2);
			if (ret) return ret;
		}
		os_delay_ms(10); // TO REMOVE
	}
	return NAU83GXX_RC_OK;
}


uint8_t update_real_device_id(struct dev_desc_t *i2c_dev,
		struct NAU83GXX_runtime_t *runtime_handle)
{
	uint8_t rc;
	uint8_t read_i2c_data[2];
	uint8_t device_id;
	uint8_t device_addr;
	uint8_t low_4bits_in_dev_addr;

	device_addr = runtime_handle->dev_addr;
	rc = nau83gxx_read(
			i2c_dev, device_addr, NAU83GXX_REG_DEVICE_ID, read_i2c_data, 2);
	if (rc) return rc;

	device_id = read_i2c_data[1] & 0xF0;

	if (NAU83GXX_CHIP_TYPE_G20 == device_id)
	{
		low_4bits_in_dev_addr = device_addr & 0x0F;
		if ((0xA == low_4bits_in_dev_addr) || (0x0B == low_4bits_in_dev_addr))
		{
			device_id = NAU83GXX_CHIP_TYPE_G60;
		}
	}
	runtime_handle->chip_type = device_id;
	return NAU83GXX_RC_OK;
}


/*
 * Subroutine:  init_83gXX_dsp
 *
 * Description:
 * Initialization of 83GXX Chip through I2C bus.
 *
 * Inputs:
 * Return:      None
 */
static uint8_t init_83gXX_dsp(struct dev_desc_t *i2c_dev,
		uint8_t device_addr,  struct NAU83GXX_runtime_t *runtime_handle)
{
	uint16_t init_array_size = 0;
	const struct NAU83GXX_reg_s *init_array;

	switch(runtime_handle->chip_type)
	{
	case NAU83GXX_CHIP_TYPE_G10:
		init_array = cmd_83G10c_init;
		init_array_size = cmd_83G10c_init_size;
		break;
	case NAU83GXX_CHIP_TYPE_G20:
		init_array = cmd_83G20a_init;
		init_array_size = cmd_83G20a_init_size;
		break;
	case NAU83GXX_CHIP_TYPE_G60:
		init_array = cmd_83G60_init;
		init_array_size = cmd_83G60_init_size;
		break;
	default:
		return NAU83GXX_RC_NOT_SUPPORTED_DEVICE_ID;
		break;
	}

	init_array_size = init_array_size / sizeof(struct NAU83GXX_reg_s);

	return send_register_array(
			i2c_dev, device_addr, init_array, init_array_size);
}


#define SOURCE_CHANNEL_POS  10
#define ENABLE_AEC_OUT_BIT      0x0001
#define ENABLE_AEC_ON_LEFT_CH   0x0004
#define ENABLE_AEC_ON_RIGHT_CH  0x0000
static uint8_t set_pcm_ctl(struct NAU83GXX_config_t *config_handle,
								struct dev_desc_t * i2c_dev, uint8_t dev_addr)
{
	uint16_t reg_0xB_data;
	uint16_t reg_0xC_data;
	uint8_t rc;

	reg_0xB_data = config_handle->I2S_in_channel << SOURCE_CHANNEL_POS;
	reg_0xC_data = 0xC00;
	switch (config_handle->I2S_out_left_ch_data_type)
	{
	default:
	case NAU83GXX_OUT_CHANNEL_MUTE:
		// do nothing keep ADC_I_SEL and ADC_I_SEL on '000'
		break;
	case NAU83GXX_OUT_CHANNEL_I_SENSE:
		CRITICAL_ERROR("cannot put Isense on channel 0");
		break;
	case NAU83GXX_OUT_CHANNEL_V_SENSE:
		reg_0xC_data |= 0x4;
		break;
	case NAU83GXX_OUT_CHANNEL_AEC:
		reg_0xB_data |= (ENABLE_AEC_OUT_BIT | ENABLE_AEC_ON_LEFT_CH);
		reg_0xC_data |= 0x4;
		break;
	}

	switch (config_handle->I2S_out_right_ch_data_type)
	{
	default:
	case NAU83GXX_OUT_CHANNEL_MUTE:
		// do nothing keep ADC_I_SEL and ADC_I_SEL on '000'
		break;
	case NAU83GXX_OUT_CHANNEL_I_SENSE:
		reg_0xC_data |= 0x20;
		break;
	case NAU83GXX_OUT_CHANNEL_V_SENSE:
		CRITICAL_ERROR("cannot put Vsense on channel 1");
		break;
	case NAU83GXX_OUT_CHANNEL_AEC:
		reg_0xB_data |= (ENABLE_AEC_OUT_BIT | ENABLE_AEC_ON_RIGHT_CH);
		reg_0xC_data |= 0x20;
		break;
	}
	rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x0B, reg_0xB_data);
	if (rc) return rc;

	return nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x0C, reg_0xC_data);
}

#define CURRENT_LIMIT_POS 11
#define CURRENT_BOOST_LIMIT_POS 8
static uint8_t set_current_and_boost_limits(
		struct NAU83GXX_config_t *config_handle,
		struct NAU83GXX_runtime_t *runtime_handle,
		struct dev_desc_t * i2c_dev, uint8_t dev_addr)
{
	uint16_t reg_0x6B_data;
	uint16_t reg_0x17_data;
	uint16_t current_limit_8ohm;

	if (NAU83GXX_CHIP_TYPE_G10 == runtime_handle->chip_type)
	{
		current_limit_8ohm = config_handle->current_limit_8ohm - 1200;
		current_limit_8ohm = current_limit_8ohm / 190;// steps of 0.19V
		reg_0x6B_data = 0x00F0;// default value
		reg_0x6B_data |= (current_limit_8ohm) << CURRENT_LIMIT_POS;

		reg_0x17_data = (config_handle->boost_margin_mv / 190) |
				((config_handle->boost_target_limit_mv / 190) <<
						CURRENT_BOOST_LIMIT_POS);
		nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x17, reg_0x17_data);
	}
	else if (NAU83GXX_CHIP_TYPE_G20 == runtime_handle->chip_type)
	{
		reg_0x6B_data = 0x0070;// default value
	}
	else
	{
		return NAU83GXX_RC_NOT_SUPPORTED_DEVICE_ID;
	}
	return nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x6B, reg_0x6B_data);
}


static void enable_recovery(struct NAU83GXX_config_t *config_handle,
						struct dev_desc_t * i2c_dev, uint8_t dev_addr)
{
	uint16_t reg_0x05_data;
	uint16_t reg_0x0A_data;

	nau83gxx_read_wordU16(i2c_dev, dev_addr, 0x05, &reg_0x05_data);
	reg_0x05_data &= ~(1 << 4); // enable APR Emergency Shutdown Interrupt
	nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x05, reg_0x05_data);

	nau83gxx_read_wordU16(i2c_dev, dev_addr, 0x0A, &reg_0x0A_data);
	reg_0x0A_data |= (1 << 11); // enable IRQ
	nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x0A, reg_0x0A_data);

	DEV_IOCTL(config_handle->irq_pin, IOCTL_DEVICE_START );
}


uint8_t nau83gxx_init_i2c_regs(struct NAU83GXX_config_t *config_handle,
								struct NAU83GXX_runtime_t *runtime_handle)
{
	uint8_t dev_addr;
	struct dev_desc_t * i2c_dev;
	uint8_t rc;

	dev_addr = runtime_handle->dev_addr;
	i2c_dev = config_handle->i2c_dev;

	//HW reset
	os_delay_ms(5);
	rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x000, 0x000);
	if (rc) return rc;

	rc = init_83gXX_dsp(i2c_dev, dev_addr, runtime_handle);
	if (rc) return rc;

	if (NAU83GXX_CHIP_TYPE_G60 == runtime_handle->chip_type)
	{
		return 0;
	}

	rc = set_pcm_ctl(config_handle, i2c_dev, dev_addr);
	if (rc) return rc;

	rc = set_current_and_boost_limits(
			config_handle, runtime_handle, i2c_dev, dev_addr);
	if (rc) return rc;

	rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x01, 0x0000); // soft reset
	if (rc) return rc;

	if (config_handle->enable_recovery)
	{
		enable_recovery(config_handle, i2c_dev, dev_addr);
	}

	// write 2 times to 0x00 for states reset
	rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x01, 0x0000);
	rc = nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x01, 0x0000);
	if (rc) return rc;
	os_delay_ms(63);
	return nau83gxx_write_wordU16(i2c_dev, dev_addr, 0x1A, 0x0020);
}
