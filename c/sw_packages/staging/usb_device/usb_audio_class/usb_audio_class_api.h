
#ifndef _USB_AUDIO_CLASS_API_H_
#define _USB_AUDIO_CLASS_API_H_


/*****************  defines  **************/



/**********  define API  types ************/
typedef enum
{
	USB_AUDIO_CLASS_IOCTL_GET_RX_BUFF = IOCTL_LAST_COMMON_IOCTL + 1,
	USB_AUDIO_CLASS_IOCTL_RELEASE_RX_BUFF,
	USB_AUDIO_CLASS_IOCTL_GET_EMPTY_TX_BUFF,
	USB_AUDIO_CLASS_IOCTL_RELEASE_TX_BUFF,
	USB_AUDIO_CLASS_IOCTL_SLOWDOWN_BY_SKIPING_SAMPLES,
	USB_AUDIO_CLASS_IOCTL_ACCELERATE_BY_REPEATING_SAMPLES,
	USB_AUDIO_CLASS_IOCTL_SOFT_RESET,
//	USB_AUDIO_CLASS_IOCTL_SET_BUFF_SIZE
} USB_AUDIO_CLASS_API_ioctl_t;


/**********  define API  functions  ************/

#endif