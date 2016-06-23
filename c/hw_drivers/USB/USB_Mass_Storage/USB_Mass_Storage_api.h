
#ifndef _USB_Mass_Storage_API_H_
#define _USB_Mass_Storage_API_H_

/*****************  defines  **************/


/**********  define API  types ************/

typedef void*  USB_Mass_Storage_API_Handle_t;


typedef enum
{
	USB_Mass_Storage_API_Chip_STM32F10x=0,
} USB_Mass_Storage_API_Chip_t;

typedef struct {
	USB_Mass_Storage_API_Chip_t chip;
	uint32_t  BlockCount;
	uint32_t  BlockSize;
	pdev_descriptor_t storage_dev;
} USB_Mass_Storage_API_Init_params_t;

/**********  define API  functions  ************/
uint8_t  USB_Mass_Storage_API_Init(USB_Mass_Storage_API_Init_params_t *apInit_params);




#else
#pragma message( "warning : this header file had already been included" )
#endif
