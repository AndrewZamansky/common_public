
#ifndef _shell_presets_API_H_
#define _shell_presets_API_H_


#include "src/_shell_presets_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_SHELL_PRESETS_START_RECORDING = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_SHELL_PRESETS_STOP_RECORDING,
	IOCTL_SHELL_PRESETS_LOAD_PRESET,
	IOCTL_SHELL_PRESETS_SET_PRESET_TO_DEFAULT
}shell_presets_api_ioctl_t;


/**********  define API  functions  ************/



#endif
