/*
 * file : spi_flash.h
 *
 *
 */

#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H


struct spi_flash_instance_t {
	struct dev_desc_t *   spi_server_dev;
	struct dev_desc_t *   gpio_select_dev;
};

#endif
