
#ifndef _equalizer_API_H_
#define _equalizer_API_H_

#include "equalizer_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "src/_equalizer_prerequirements_check.h" // should be after {equalizer_config.h,dev_managment_api.h}

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_EQUALIZER_SET_BAND_BIQUADS = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_EQUALIZER_SET_NUM_OF_BANDS,
}EQUALIZER_API_ioctl_t;


typedef struct _set_band_biquads_t
{
	uint32_t band_num;
	biquads_filter_mode_t filter_mode;
	float Fc;
	float QValue;
	float Gain;

} set_band_biquads_t;

/**********  define API  functions  ************/


uint8_t  equalizer_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor);

//#include "src/_equalizer_static_dev_macros.h"
//
//#define EQUALIZER_API_CREATE_STATIC_DEV(dev,dev_name)  __EQUALIZER_API_CREATE_STATIC_DEV(dev,dev_name )
//

#endif
