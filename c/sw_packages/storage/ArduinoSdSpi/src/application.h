/*
 * file : u_boot_shell.h
 *
 *
 */

#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "dev_management_api.h"


/***************   typedefs    *******************/

//struct u_boot_shell_instance_t{
//	struct dev_desc_t * server_dev;
//};

class SPISettings {
public:
	SPISettings(int, int,int) {}
	SPISettings() {}
};

// called from yield function. should not be used, but needs to be implemented
class Particle_class {
	public:
	void process() {}
};

class SPI_class {
public:
	void beginTransaction(SPISettings m_spiSettings) {}
	void endTransaction() {}
	void begin() {}
	int transfer(int value)
	{
		return 1;
	}
};

extern SPI_class arduino_spi_inst;
extern Particle_class Particle;

extern uint32_t micros();
extern int millis();
extern bool digitalWrite(int pin, int logic);
extern void pinMode(uint8_t pin, uint8_t mode);

#define SDCARD_SPI    arduino_spi_inst
#define SPI_MODE0     0
#define MSBFIRST      0

#endif
