
#ifndef _biquad_filter_API_H_
#define _biquad_filter_API_H_

#include "src/_biquad_filter_prerequirements_check.h"


/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  types ************/
typedef enum _biquads_filter_mode_t {
    BIQUADS_TRANSPARENT_MODE,
    BIQUADS_LOWPASS_MODE_1_POLE,
    BIQUADS_LOWPASS_MODE_2_POLES,
    BIQUADS_HIGHPASS_MODE_1_POLE,
    BIQUADS_HIGHPASS_MODE_2_POLES,
    BIQUADS_BANDPASS_MODE,
    BIQUADS_PEAK_MODE,
    BIQUADS_LOWSHELF_MODE,
    BIQUADS_HIGHSHELF_MODE,

} biquads_filter_mode_t;





typedef enum
{
	IOCTL_BIQUAD_FILTER_SET_BAND_BIQUADS = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS,
	IOCTL_BIQUAD_FILTER_GET_BAND_BIQUADS
}BIQUAD_FILTER_API_ioctl_t;


typedef struct _biquad_filter_api_band_set_params_t
{
	float Fc;
	float QValue;
	float Gain;
	biquads_filter_mode_t filter_mode;

} biquad_filter_api_band_set_params_t;

typedef struct _biquad_filter_api_band_set_t
{
	biquad_filter_api_band_set_params_t  band_set_params;
	uint8_t band_num;
} biquad_filter_api_band_set_t;

/**********  define API  functions  ************/


extern char biquad_filter_module_name[] ;
#define BIQUAD_FILTER_API_MODULE_NAME	biquad_filter_module_name


#endif
