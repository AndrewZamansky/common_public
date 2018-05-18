
#ifndef _DPWM_I94XXX_API_H_
#define _DPWM_I94XXX_API_H_

#include "dev_management_api.h"
//#include "src/dpwm.h"

/*****************  defines  **************/

#define  DPWM_IO_ON_GPA 0
#define  DPWM_IO_ON_GPC 1
#define  DPWM_IO_ON_GPD 2

#define    DPWM_I94XXX_API_CH_IO_BANK_A   DPWM_IO_ON_GPA
#define    DPWM_I94XXX_API_CH_IO_BANK_C   DPWM_IO_ON_GPC
#define    DPWM_I94XXX_API_CH_IO_BANK_D   DPWM_IO_ON_GPD

/**********  define API  types ************/

typedef enum
{
	DPWM_I94XXX_ENABLE_OUTPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	DPWM_I94XXX_GET_MEASURED_SAMPLE_RATE,
	DPWM_I94XXX_DEVICE_STOP
}DPWM_I94XXX_API_ioctl_t;



/**********  define API  functions  ************/

#endif
