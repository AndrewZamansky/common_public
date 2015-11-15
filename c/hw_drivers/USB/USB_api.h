
#ifndef _USB_API_H_
#define _USB_API_H_

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	USB_API_Chip_STM32F10x=0,
} USB_API_Chip_t;


typedef struct {
	USB_API_Chip_t chip;
} USB_API_Init_params_t;

/**********  define API  functions  ************/
uint8_t  USB_API_Init(USB_API_Init_params_t *apInit_params);


#else
#pragma message( "warning : this header file had already been included" )
#endif
