/*
 *
 * file :   ArduinoSdSpi.cpp
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

extern "C" {
	#include "dev_management_api.h"
}

#include "ArduinoSdSpi_api.h"

#include "application.h"
#include "ArduinoSdSpi.h"
#include "management_api.h"
#include "os_wrapper.h"

#include "_ArduinoSdSpi_prerequirements_check.h"

#include "SdSpiCard.h"

extern "C" {
	#include "hw_timer_api.h"
	#include "spi_i94xxx_api.h"
}
/*following line add module to available module list for dynamic device tree*/
//#include "ArduinoSdSpi_add_component.h"


/********  defines *********************/


#define MAX_NUMBER_OF_ARDUINO_SPI_SD_INSTANCES   2
uint8_t g_num_dev_inst = 0;

/********  types  *********************/

struct lookup_table_t {
	struct dev_desc_t *spi_dev;
};

lookup_table_t lookup_table[MAX_NUMBER_OF_ARDUINO_SPI_SD_INSTANCES] = {NULL};

/********  externals *********************/

extern "C" {



	static SdSpiCardEX *SdSpiCard_inst;



	uint8_t cSdSpiCardEX_begin(SdSpiDriver* spi, uint8_t csPin, SPISettings spiSettings)
	{
		if(SdSpiCard_inst->begin(spi, csPin, spiSettings))
			return 1;
		else return 0;
	}



	uint8_t ArduinoSdSpi_pwrite(struct dev_desc_t *adev,  //Device in .h file
			const uint8_t *apData,  /* Data buffer to write data from */
			size_t aLength, 		/* Amount of data to write in blocks*512 */
			size_t  aOffset)   		/* Position*512 to write into device*/
	{
		struct arduino_sd_spi_runtime_t *runtime_handle;

		runtime_handle = (struct arduino_sd_spi_runtime_t *)
							    DEV_GET_RUNTIME_DATA_POINTER(adev);

		// This should be handled by setup
		//config_handle->sd_spi_inst = (void*) (new SdSpiCardEX);

		SdSpiCard_inst = (SdSpiCardEX *)runtime_handle->sd_spi_inst;


		/**  bool writeBlocks(uint32_t block, const uint8_t* src, size_t nb);
		* Write multiple 512 byte blocks to an SD card.
		*
		* \param[in] block Logical block to be written.
		* \param[in] nb Number of blocks to be written.
		* \param[in] src Pointer to the location of the data to be written.
		* \return The value true is returned for success and
		* the value false is returned for failure.
		*/

		if (SdSpiCard_inst->writeBlocks(
				aOffset / 512, (uint8_t *)apData, (size_t)(aLength / 512)))
		{
			return 1;
		}
		else return 0;


	}

	uint8_t ArduinoSdSpi_pread(struct dev_desc_t *adev,  //Device in .h file
			uint8_t *apData,  /* Data buffer to read data into */
			size_t aLength, 		/* Amount of data to read in blocks*512 */
			size_t  aOffset)   		/* Position*512 to read from device*/
	{
		struct arduino_sd_spi_runtime_t *runtime_handle;

		runtime_handle = (struct arduino_sd_spi_runtime_t *)
							    DEV_GET_RUNTIME_DATA_POINTER(adev);

		// This should be handled by setup
		//config_handle->sd_spi_inst = (void*) (new SdSpiCardEX);

		SdSpiCard_inst = (SdSpiCardEX *)runtime_handle->sd_spi_inst;


		/**  bool readBlocks(uint32_t block, uint8_t* dst, size_t nb);
		* Read multiple 512 byte blocks from an SD card.
		*
		* \param[in] block Logical block to be read.
		* \param[in] nb Number of blocks to be read.
		* \param[out] dst Pointer to the location that will receive the data.
		* \return The value true is returned for success and
		* the value false is returned for failure.
		*/

		if (SdSpiCard_inst->readBlocks(
				aOffset / 512, (uint8_t *)apData, (size_t)(aLength / 512)))
		{
			return 1;
		}
		else return 0;
	}
}

Particle_class Particle;

SPI_class arduino_spi_inst;


// should not be used, but needs to be implemented
bool digitalWrite(int pin, int logic)
{
	if (HIGH == logic)
	{
		DEV_IOCTL_0_PARAMS(
				lookup_table[pin].spi_dev, IOCTL_SPI_API_SET_CS_HIGH);
	}
	else if (LOW == logic)
	{
		DEV_IOCTL_0_PARAMS(
				lookup_table[pin].spi_dev, IOCTL_SPI_API_SET_CS_LOW);
	}
	return 1;
}

// should not be used, but needs to be implemented
void pinMode(uint8_t pin, uint8_t mode)
{
}

// systick is a struct that has something to increment every millisecond
// TODO : move to instance member ; need to be moved to timer wrapper
extern struct dev_desc_t *systick_dev;

int millis()
{
	uint64_t curr_counter;

	DEV_IOCTL(systick_dev,
				IOCTL_GET_CURRENT_TIMER_VALUE, (void*)&curr_counter);
	return (uint32_t)curr_counter;
}

// needed only to call sysCalls::yield (not needed for now)
uint32_t micros()
{
	return 0;
}


void SdSpiAltDriver::begin(uint8_t csPin)
{
	m_csPin = csPin;
}


void SdSpiAltDriver::activate()
{
}

void SdSpiAltDriver::deactivate()
{
}


uint8_t SdSpiAltDriver::receive()
{
	uint8_t data;
	receive(&data, 1);
	return data;
}


uint8_t SdSpiAltDriver::receive(uint8_t* buf, size_t n)
{

	DEV_READ(lookup_table[this->m_csPin].spi_dev, buf, n);

	return 0;
}


void SdSpiAltDriver::send(uint8_t data)
{
	send(&data, 1);
}



void SdSpiAltDriver::send(const uint8_t* buf, size_t n)
{

	DEV_WRITE(lookup_table[this->m_csPin].spi_dev, buf, n);

}


extern "C" {
	uint8_t ArduinoSdSpi_ioctl( struct dev_desc_t *adev ,
		const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);
}



/**
 * ArduinoSdSpi_ioctl()
 *
 * return:
 */
uint8_t ArduinoSdSpi_ioctl( struct dev_desc_t *adev ,
		const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	struct arduino_sd_spi_cfg_t *config_handle;
	struct arduino_sd_spi_runtime_t *runtime_handle;
	struct dev_desc_t *spi_dev;
	uint8_t num_dev_inst;

	config_handle =
		(struct arduino_sd_spi_cfg_t *)DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_handle =
		(struct arduino_sd_spi_runtime_t *)DEV_GET_RUNTIME_DATA_POINTER(adev);
	spi_dev = config_handle->spi_dev;
	uint32_t clk_freq = (uint32_t)config_handle->clk_freq;


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :

		//Wait 1ms for sdcard to power up at minimum.
		os_delay_ms(1);

		DEV_IOCTL_0_PARAMS(spi_dev , IOCTL_DEVICE_START );

		if ((NULL == runtime_handle->sd_spi_inst) &&
				 (MAX_NUMBER_OF_ARDUINO_SPI_SD_INSTANCES > g_num_dev_inst))
		{

			num_dev_inst = g_num_dev_inst++;
			runtime_handle->num_dev_inst = num_dev_inst;
			runtime_handle->sd_spi_inst = (void *) (new SdSpiCardEX);
			runtime_handle->SdSpiAltDriver_obj = (void *) (new SdSpiAltDriver);

			lookup_table[num_dev_inst].spi_dev = spi_dev;
			SdSpiCard_inst = (SdSpiCardEX *)runtime_handle->sd_spi_inst;
		}
		num_dev_inst = runtime_handle->num_dev_inst ;

		{
		SPISettings spiSettings(0,0,0);
		cSdSpiCardEX_begin(
				(SdSpiDriver *)runtime_handle->SdSpiAltDriver_obj,
				num_dev_inst, spiSettings);
		}
		DEV_IOCTL_1_PARAMS(spi_dev , IOCTL_SPI_API_SET_CLK, (void *)clk_freq );
		break;

	default :
		return 1;
	}
	return 0;
}
