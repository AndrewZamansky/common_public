
#ifndef _GPIO_API_H_
#define _GPIO_API_H_

/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  functions  ************/
typedef enum
{
	IOCTL_GPIO_PIN_SET = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_GPIO_PIN_CLEAR,
	IOCTL_GPIO_PIN_READ,
	IOCTL_LAST_COMMON_GPIO_IOCTL
}GPIO_COMMON_API_ioctl_t;




#endif
