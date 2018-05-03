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

#include "_ArduinoSdSpi_prerequirements_check.h"

#include "SdSpiCard.h"

extern "C" {
	#include "hw_timer_api.h"
	#include "spi_i94xxx_api.h"
}
/*following line add module to available module list for dynamic device tree*/
//#include "ArduinoSdSpi_add_component.h"


/********  defines *********************/


/********  types  *********************/




/********  externals *********************/

static struct dev_desc_t *l_spi_dev;


extern "C" {



	static SdSpiCardEX *SdSpiCard_inst;



	uint8_t cSdSpiCardEX_begin(SdSpiDriver* spi, uint8_t csPin, SPISettings spiSettings)
	{
		if(SdSpiCard_inst->begin(spi, csPin, spiSettings))
			return 1;
		else return 0;
	}


	uint8_t cSdSpiCardEX_syncBlocks()
	{
		if(SdSpiCard_inst->syncBlocks())
			return 1;
		else return 0;
	}

	uint8_t cSdSpiCardEX_readBlock(uint32_t block, uint8_t *dst)
	{
		if(SdSpiCard_inst->readBlock(block, dst))
			return 1;
		else return 0;
	}

	uint8_t cSdSpiCardEX_readBlocks(uint32_t block, uint8_t *dst, size_t nb)
	{
		if(SdSpiCard_inst->readBlocks(block, dst, nb))
			return 1;
		else return 0;
	}

	uint8_t cSdSpiCardEX_writeBlock(uint32_t block, const uint8_t *src)
	{
		if(SdSpiCard_inst->writeBlock(block, src))
			return 1;
		else return 0;
	}

	uint8_t cSdSpiCardEX_writeBlocks(uint32_t block, const uint8_t *src, size_t nb)
	{
		if(SdSpiCard_inst->writeBlocks(block, src, nb))
			return 1;
		else return 0;
	}

}

Particle_class Particle;

SPI_class arduino_spi_inst;

// should not be used, but needs to be implemented
bool digitalWrite(int pin, int logic)
{
	if(HIGH == logic)
	{
		DEV_IOCTL_0_PARAMS(l_spi_dev, IOCTL_SPI_API_SET_CS_HIGH);
	}
	else
	{
		DEV_IOCTL_0_PARAMS(l_spi_dev, IOCTL_SPI_API_SET_CS_LOW);
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

	DEV_READ(l_spi_dev, &data, 1);
	return data;
}


uint8_t SdSpiAltDriver::receive(uint8_t* buf, size_t n)
{
	DEV_READ(l_spi_dev, buf, n);
	return 0;
}


void SdSpiAltDriver::send(uint8_t data)
{
	DEV_WRITE(l_spi_dev, &data, 1);
}



void SdSpiAltDriver::send(const uint8_t* buf, size_t n)
{
	DEV_WRITE(l_spi_dev, buf, n);
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
	struct dev_desc_t *spi_dev;
	//SPI_T *spi_sd;

	config_handle =
			(struct arduino_sd_spi_cfg_t *)DEV_GET_CONFIG_DATA_POINTER(adev);
	spi_dev = config_handle->spi_dev;
	//spi_sd = (SPI_T *)config_handle->base_address;


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		l_spi_dev = spi_dev;
		config_handle->sd_spi_inst = (void*) (new SdSpiCardEX);
		SdSpiCard_inst = (SdSpiCardEX*)config_handle->sd_spi_inst;

		{
			SPISettings spiSettings(0, 0, 0);
			cSdSpiCardEX_begin(NULL, 0, spiSettings);
		}


		//SPI_DisableAutoSS(spi_sd);	//doesn't work

		//defined in
//		SPI_SET_SS_LOW_SPI0();
//		SPI_SET_SS_HIGH_SPI0();
		break;

	default :
		return 1;
	}
	return 0;
}
