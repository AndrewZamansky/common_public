/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "diskio.h"		/* FatFs lower layer API */
//#include "BuiltInFlash_API.h"
#include "SPI_FLASH_api.h"
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
//#include "sdcard.h"		/* Example: MMC/SDC contorl */

/* Definitions of physical drive number for each drive */

//#define ATA		0	/* Example: Map ATA drive to drive number 0 */
//#define MMC		1	/* Example: Map MMC/SD card to drive number 1 */
//#define USB		2	/* Example: Map USB drive to drive number 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;

//	int result;
//
//	switch (pdrv) {
//	case ATA :
//		result = ATA_disk_status();
//
//		// translate the reslut code here
//
//		return stat;
//
//	case MMC :
//		result = MMC_disk_status();
//
//		// translate the reslut code here
//
//		return stat;
//
//	case USB :
//		result = USB_disk_status();
//
//		// translate the reslut code here
//
//		return stat;
//	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;

//	int result;
//
//	switch (pdrv) {
//	case ATA :
//		result = ATA_disk_initialize();
//
//		// translate the reslut code here
//
//		return stat;
//
//	case MMC :
//		result = MMC_disk_initialize();
//
//		// translate the reslut code here
//
//		return stat;
//
//	case USB :
//		result = USB_disk_initialize();
//
//		// translate the reslut code here
//
//		return stat;
//	}
	return stat;
}


#if 0
extern BuiltInFlash_API_Handle_t pBuiltInFlashHandle;
#else
//extern SPI_FLASH_API_Handle_t spiFlashHandle;
extern struct dev_desc_t * spi_flash_manager_dev;
#endif

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
#if 0
	BuiltInFlash_API_ReadData(pBuiltInFlashHandle,sector*512,buff,count*512);
#else
	DEV_PREAD(spi_flash_manager_dev,buff,count*512,sector*512);
//	SPI_FLASH_API_ReadData(spiFlashHandle,sector*512,buff,count*512);
#endif
//	DRESULT res;
//	int result;
//
//	switch (pdrv) {
//	case ATA :
//		// translate the arguments here
//
//		result = ATA_disk_read(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//
//	case MMC :
//		// translate the arguments here
//
//		result = MMC_disk_read(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//
//	case USB :
//		// translate the arguments here
//
//		result = USB_disk_read(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//	}

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
//	DRESULT res;
//	int result;
//
#if 0
	BuiltInFlash_API_WriteData(pBuiltInFlashHandle,sector*512,buff,count*512);
#else
	DEV_PWRITE(spi_flash_manager_dev,buff,count*512,sector*512);
//	SPI_FLASH_API_WriteData(spiFlashHandle,sector*512,buff,count*512);
#endif

//	switch (pdrv) {
//	case ATA :
//		// translate the arguments here
//
//		result = ATA_disk_write(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//
//	case MMC :
//		// translate the arguments here
//
//		result = MMC_disk_write(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//
//	case USB :
//		// translate the arguments here
//
//		result = USB_disk_write(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//	}

	return RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
//	DRESULT res;
//	int result;
//
//	switch (pdrv) {
//	case ATA :
//
//		// Process of the command for the ATA drive
//
//		return res;
//
//	case MMC :
//
//		// Process of the command for the MMC/SD card
//
//		return res;
//
//	case USB :
//
//		// Process of the command the USB drive
//
//		return res;
//	}

	return RES_PARERR;
}
#endif

DWORD get_fattime (void)
{
	return 0;
}
