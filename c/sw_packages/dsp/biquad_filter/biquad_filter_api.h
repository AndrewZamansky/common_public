
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
	IOCTL_BIQUAD_FILTER_SET_BAND_RAW_COEFFICIENTS,
	IOCTL_BIQUAD_FILTER_SET_NUM_OF_BANDS,
	IOCTL_BIQUAD_FILTER_GET_BAND
};


struct biquad_filter_api_band_set_params_t
{
	float Fc;
	float QValue;
	float Gain_db;
	float Sample_rate;
	enum biquads_filter_mode_e filter_mode;
	uint8_t bypass;
};



enum biquad_filter_api_raw_coeff_type_e {
	BIQUAD_FILTER_RAW_COEF_TYPE_FLOATING_POINT,
	BIQUAD_FILTER_RAW_COEF_TYPE_FIX_POINT,
};

struct biquad_filter_api_raw_coeff_floating_point_t
{
	float b0;
	float b1;
	float b2;
	float a1;
	float a2;
};

struct biquad_filter_api_raw_coeff_fix_point_t
{
	int16_t num_of_fraction_bits;
	int32_t b0;
	int32_t b1;
	int32_t b2;
	int32_t a1;
	int32_t a2;
};

struct biquad_filter_api_band_set_t
{
	struct biquad_filter_api_band_set_params_t  band_set_params;
	uint8_t band_num;
};

struct biquad_filter_api_band_set_raw_t
{
	uint8_t band_num;
	enum biquad_filter_api_raw_coeff_type_e raw_coeff_type;
	union {
		struct biquad_filter_api_raw_coeff_floating_point_t raw_coeff_float;
		struct biquad_filter_api_raw_coeff_fix_point_t   raw_coeff_fixed_point;
	};
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
