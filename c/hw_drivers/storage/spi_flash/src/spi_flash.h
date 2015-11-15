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
	pdev_descriptor_const   spi_server_dev;
	pdev_descriptor_const   gpio_select_dev;
} spi_flash_Instance_t;

#endif /* */
