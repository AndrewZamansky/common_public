
#ifndef _downsampling_by_int_filter_API_H_
#define _downsampling_by_int_filter_API_H_

#include "dsp_management_api.h"


enum DOWNSAMPLING_BY_INT_FILTER_API_ioctl_e {
	IOCTL_DOWNSAMPLING_BY_INT_SET_PARAMS = IOCTL_DSP_LAST_COMMON_IOCTL + 1
};

struct downsampling_by_int_api_set_params_t {
	uint32_t  input_sample_rate_Hz;
	uint32_t  output_sample_rate_Hz;

	// should be odd for linear delay
	size_t number_of_coefficients_in_lowpass_filter;

	size_t expected_number_of_input_samples;// relevant to ARM CMSIS
};


#ifdef __cplusplus
extern "C" {
#endif

extern char downsampling_by_int_module_name[] ;
#define DOWNSAMPLING_BY_INT_DSPT  downsampling_by_int_module_name

#ifdef __cplusplus
}
#endif


#endif
