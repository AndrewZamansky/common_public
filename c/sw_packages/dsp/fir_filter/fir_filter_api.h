
#ifndef _fir_filter_API_H_
#define _fir_filter_API_H_

#include "dsp_management_api.h"

enum FIR_FILTER_API_ioctl_e {
	IOCTL_FIR_FILTER_SET_FIR_COEFFICIENTS = IOCTL_DSP_LAST_COMMON_IOCTL+1
};

struct fir_filter_api_set_params_t {
	float	 *p_coefficients;
	size_t number_of_filter_coefficients ;
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
