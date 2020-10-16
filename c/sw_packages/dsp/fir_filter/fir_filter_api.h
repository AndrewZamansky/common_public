
#ifndef _fir_filter_API_H_
#define _fir_filter_API_H_

#include "dsp_management_api.h"


enum fir_filter_mode_e {
    FIR_TRANSPARENT_MODE,
    FIR_LOWPASS_MODE,
    FIR_HIGHPASS_MODE,
    FIR_BANDPASS_MODE,
    FIR_NOTCH_MODE,
    FIR_HILBERT_TRANSFORM_MODE
};

enum fir_filter_set_type_e {
    FIR_SET_RAW_COEFFICIENTS,
    FIR_CALCULATE_COEFFICIENTS_BY_PARAMS
};

enum FIR_FILTER_API_ioctl_e {
	IOCTL_FIR_FILTER_SET_FIR_COEFFICIENTS = IOCTL_DSP_LAST_COMMON_IOCTL+1
};


struct fir_filter_api_set_raw_coefficients_t {
	float  *p_coefficients;
};

struct fir_filter_api_set_coefficients_by_params_t {
	enum fir_filter_mode_e  filter_mode;
	float  fc; // requency where it happens
	float  A_stop; // stopband attenuation in dB
	float  dfc; // half-filter bandwidth (only for bandpass and notch)
	float  sample_rate;
};


struct fir_filter_api_set_params_t {
	enum fir_filter_set_type_e  set_coefficients_type;
	union {
		struct fir_filter_api_set_raw_coefficients_t  raw_coefficients;
		struct fir_filter_api_set_coefficients_by_params_t  coeff_by_params;
	};
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size ;
};


#ifdef __cplusplus
extern "C" {
#endif

extern char fir_filter_module_name[] ;
#define FIR_FILTER_DSPT	fir_filter_module_name

#ifdef __cplusplus
}
#endif
#endif
