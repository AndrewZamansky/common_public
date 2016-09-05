
#ifndef _equalizer_API_H_
#define _equalizer_API_H_

#include "src/_equalizer_prerequirements_check.h"


/*****************  defines  **************/


/**********  define API  types ************/



typedef enum
{
	IOCTL_EQUALIZER_SET_BAND_BIQUADS = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_EQUALIZER_SET_NUM_OF_BANDS,
	IOCTL_EQUALIZER_GET_BAND_BIQUADS
}EQUALIZER_API_ioctl_t;


typedef struct _equalizer_api_band_set_params_t
{
	float Fc;
	float QValue;
	float Gain;
	biquads_filter_mode_t filter_mode;

} equalizer_api_band_set_params_t;

typedef struct _equalizer_api_band_set_t
{
	equalizer_api_band_set_params_t  band_set_params;
	uint8_t band_num;
} equalizer_api_band_set_t;

/**********  define API  functions  ************/

extern char equalizer_module_name[] ;
#define EQUALIZER_API_MODULE_NAME	equalizer_module_name


#endif
