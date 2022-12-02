
#ifndef _downsampling_by_int_filter_API_H_
#define _downsampling_by_int_filter_API_H_

#include "dsp_management_api.h"
#include "biquad_filter_api.h"


enum DOWNSAMPLING_BY_INT_FILTER_API_ioctl_e {
	IOCTL_DOWNSAMPLING_BY_INT_SET_PARAMS = IOCTL_DSP_LAST_COMMON_IOCTL + 1
};

enum downsampling_filter_type_e {
	DOWNSAMPLING_BIQUAD_FILTER,
	DOWNSAMPLING_FIR_FILTER
};

struct downsampling_by_int_api_set_params_t {
	uint32_t  input_sample_rate_Hz;
	uint32_t  output_sample_rate_Hz;

	enum downsampling_filter_type_e filter_type;

	size_t num_of_bands;

	/******* For IIR low pass filter:  ******/
	/*   raw_coeffs
	 *
	 * The coefficients should correspond to the following biquads convention:
	 * y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
	 *
	 *    The coefficients are stored in the array in the following order:
	 *    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
	 */
	struct biquad_filter_api_band_set_raw_coefficients_t *raw_coeffs;
    /****** end of IIR filter section******/

	/******* For FIR low pass filter:  ******/
	// num of taps should be odd for linear delay:
	size_t number_of_coefficients_in_lowpass_filter;
	size_t expected_number_of_input_samples; // relevant to ARM CMSIS
    /****** end of IIR filter section******/
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
