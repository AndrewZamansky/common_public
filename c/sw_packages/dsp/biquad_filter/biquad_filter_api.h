
#ifndef _biquad_filter_API_H_
#define _biquad_filter_API_H_

#include "dsp_management_api.h"

enum biquads_filter_mode_e {
    BIQUADS_TRANSPARENT_MODE,
    BIQUADS_LOWPASS_MODE_1_POLE,
    BIQUADS_LOWPASS_MODE_2_POLES,
    BIQUADS_HIGHPASS_MODE_1_POLE,
    BIQUADS_HIGHPASS_MODE_2_POLES,
    BIQUADS_BANDPASS_MODE,
    BIQUADS_PEAK_MODE,
    BIQUADS_LOWSHELF_MODE,
    BIQUADS_HIGHSHELF_MODE,
    BIQUADS_ALL_PASS_BUTTERWORTH_1_POLE,
    BIQUADS_ALL_PASS_BUTTERWORTH_2_POLES,
};


enum BIQUAD_FILTER_API_ioctl_e {
	IOCTL_BIQUAD_FILTER_SET_BAND = IOCTL_DSP_LAST_COMMON_IOCTL+1,
	IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS,
	IOCTL_BIQUAD_FILTER_GET_BAND
};


struct biquad_filter_api_band_set_params_t
{
	float Fc;
	float QValue;
	float Gain_db;
	enum biquads_filter_mode_e filter_mode;
	uint8_t bypass;
};

struct biquad_filter_api_band_set_t
{
	struct biquad_filter_api_band_set_params_t  band_set_params;
	uint8_t band_num;
};


#ifdef __cplusplus
extern "C" {
#endif

extern char biquad_filter_module_name[] ;
#define BIQUAD_FILTER_DSPT	biquad_filter_module_name

#ifdef __cplusplus
}
#endif

#endif
