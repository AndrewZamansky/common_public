
#ifndef _ram_disk_API_H_
#define _ram_disk_API_H_

/*****************  defines  **************/


/**********  define API  types ************/

typedef void*  ram_disk_API_Handle_t;

typedef enum
{
	RAM_DISK_API_Chip_STM32F10x=0,
}RAM_DISK_API_Chip_t;

typedef struct {
	ram_disk_API_Chip_t chip;
} ram_disk_API_Init_params_t;


/**********  define API  functions  ************/
uint8_t  ram_disk_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);




#endif
