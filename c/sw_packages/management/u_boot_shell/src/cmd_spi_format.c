/*
 *  cmd_spi_format.c
 */
#include "_project.h"

#include <u-boot/include/command.h>
#include "u_boot_shell_api.h"
#include "SPI_FLASH_PARTITION_MANAGER_api.h"

extern struct dev_desc_t * spi_flash_manager_dev;

/*
 * Subroutine:  do_spi_format
 *
 * Description:
 *
 * Inputs:
 *
 * Return:      None
 *
 */
static int do_spi_format(
		cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	DEV_IOCTL(spi_flash_manager_dev,
				IOCTL_SPI_FLASH_PARTITION_MANAGER_FORMAT , 0);

	return 0;
}

U_BOOT_CMD(
	spi_format, 255, 0, do_spi_format,
	"",
	"info   - \n"
);

