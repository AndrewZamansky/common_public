
#ifndef _ram_disk_API_H_
#define _ram_disk_API_H_


typedef void*  ram_disk_API_Handle_t;

enum RAM_DISK_API_Chip_e {
	RAM_DISK_API_Chip_STM32F10x = 0,
};

struct ram_disk_API_Init_params_t {
	enum ram_disk_API_Chip_e chip;
};


uint8_t  ram_disk_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);

#endif
