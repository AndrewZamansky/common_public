
#ifndef _downsampling_by_int_filter_API_H_
#define _downsampling_by_int_filter_API_H_

#include "dsp_management_api.h"


enum DOWNSAMPLING_BY_INT_FILTER_API_ioctl_e {
	IOCTL_DOWNSAMPLING_BY_INT_SET_FACTOR = IOCTL_DSP_LAST_COMMON_IOCTL + 1,
	IOCTL_DOWNSAMPLING_BY_INT_SET_FIR_COEFFICIENTS
};

struct downsampling_by_int_api_set_params_t {
	float	 *p_coefficients;
	size_t number_of_filter_coefficients ;
	size_t predefined_data_block_size ;
};


#ifdef __cplusplus
extern "C" {
#endif

extern char downsampling_by_int_module_name[] ;
#define DOWNSAMPLING_BY_INT_DSPT	downsampling_by_int_module_name

#ifdef __cplusplus
}
#endif


#endif
