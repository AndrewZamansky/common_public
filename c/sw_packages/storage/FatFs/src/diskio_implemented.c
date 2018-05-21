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
#include "dev_management_api.h"

#include "diskio.h"		/* FatFs lower layer API */

/* Definitions of physical drive number for each drive */

//#define ATA		0	/* Example: Map ATA drive to drive number 0 */
//#define MMC		1	/* Example: Map MMC/SD card to drive number 1 */
//#define USB		2	/* Example: Map USB drive to drive number 2 */


extern struct dev_desc_t *storage_dev;

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
//		// translate the result code here
//
//		return stat;
//
//	case MMC :
//		result = MMC_disk_status();
//
//		// translate the result code here
//
//		return stat;
//
//	case USB :
//		result = USB_disk_status();
//
//		// translate the result code here
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
//		// translate the result code here
//
//		return stat;
//
//	case MMC :
//		result = MMC_disk_initialize();
//
//		// translate the result code here
//
//		return stat;
//
//	case USB :
//		result = USB_disk_initialize();
//
//		// translate the result code here
//
//		return stat;
//	}
	return stat;
}



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

	DEV_PREAD(storage_dev, buff, count * 512, sector * 512);

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

	DEV_PWRITE(storage_dev, buff, count * 512, sector * 512);


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
