
#ifndef _DMIC_I94XXX_API_H_
#define _DMIC_I94XXX_API_H_

#include "I94100.h"

/*****************  defines  **************/


enum  dmic_i94xxx_pinout_e {
	DMIC_I94XXX_DMIC_DATA0_A0,
	DMIC_I94XXX_DMIC_CLK0_A1,
	DMIC_I94XXX_DMIC_DATA1_A2,
	DMIC_I94XXX_DMIC_CLK1_A3,
};

enum DMIC_I94XXX_API_ioctl_e
{
	DMIC_I94XXX_ENABLE_INPUT_IOCTL = IOCTL_LAST_COMMON_IOCTL + 1,
	DMIC_I94XXX_DISABLE_INPUT_IOCTL
};


/**********  define API  types ************/



/**********  define API  functions  ************/

#endif
