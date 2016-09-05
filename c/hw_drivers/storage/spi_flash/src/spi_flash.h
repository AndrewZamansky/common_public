/*
 * file : spi_flash.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H


/***************   typedefs    *******************/

typedef struct {
	pdev_descriptor_t   spi_server_dev;
	pdev_descriptor_t   gpio_select_dev;
} spi_flash_instance_t;

#endif /* */
