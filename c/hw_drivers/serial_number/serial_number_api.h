
#ifndef _SERIAL_NUM_API_H_
#define _SERIAL_NUM_API_H_

/*****************  defines  **************/

#define SERIAL_NUMBER_LEN	16

/**********  define API  types ************/


/**********  define API  functions  ************/
typedef enum
{
	IOCTL_SERIAL_NUM_GET = IOCTL_LAST_COMMON_IOCTL+1

}SERIAL_NUM_COMMON_API_ioctl_t;



#endif
