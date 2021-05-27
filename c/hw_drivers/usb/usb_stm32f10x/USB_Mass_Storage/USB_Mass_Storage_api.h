
#ifndef _USB_Mass_Storage_API_H_
#define _USB_Mass_Storage_API_H_


typedef void*  USB_Mass_Storage_API_Handle_t;


enum USB_Mass_Storage_API_Chip_e {
	USB_Mass_Storage_API_Chip_STM32F10x = 0,
};

struct USB_Mass_Storage_API_Init_params_t {
	enum USB_Mass_Storage_API_Chip_e chip;
	uint32_t  BlockCount;
	uint32_t  BlockSize;
	struct dev_desc_t * storage_dev;
};

uint8_t  USB_Mass_Storage_API_Init(struct USB_Mass_Storage_API_Init_params_t *apInit_params);

#endif
