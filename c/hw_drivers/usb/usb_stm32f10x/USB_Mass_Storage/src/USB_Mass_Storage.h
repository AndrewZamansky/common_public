/*
 * file : USB_Mass_Storage.h
 *
 */

#ifndef _USB_Mass_Storage_H
#define _USB_Mass_Storage_H


struct USB_Mass_Storage_Instance_t {
	USB_Mass_Storage_API_Chip_t chip;
	void *pStorageInstance;
	uint32_t  BlockCount;
	uint32_t  BlockSize;
	struct dev_desc_t * storage_dev;
};



#endif
