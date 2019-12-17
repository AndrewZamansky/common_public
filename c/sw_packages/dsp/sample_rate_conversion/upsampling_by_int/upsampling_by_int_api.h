
#ifndef _upsampling_by_int_filter_API_H_
#define _upsampling_by_int_filter_API_H_

#include "dsp_management_api.h"


enum UPSAMPLING_BY_INT_FILTER_API_ioctl_e {
	IOCTL_UPSAMPLING_BY_INT_SET_FACTOR = IOCTL_DSP_LAST_COMMON_IOCTL + 1 ,
	IOCTL_UPSAMPLING_BY_INT_SET_FIR_COEFFICIENTS
};

struct upsampling_by_int_api_set_params_t {
	float	 *p_coefficients;
	size_t number_of_filter_coefficients ;
	size_t predefined_data_block_size ;
};

#ifdef __cplusplus
extern "C" {
#endif

extern char upsampling_by_int_module_name[] ;
#define UPSAMPLING_BY_INT_DSPT	upsampling_by_int_module_name

#ifdef __cplusplus
}
#endif

#endif
