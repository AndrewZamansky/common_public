
#ifndef _fir_filter_API_H_
#define _fir_filter_API_H_

#include "dsp_management_api.h"


/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  types ************/

typedef enum
{
	IOCTL_FIR_FILTER_SET_FIR_COEFFICIENTS = IOCTL_DSP_LAST_COMMON_IOCTL+1
}FIR_FILTER_API_ioctl_t;

typedef struct _fir_filter_api_band_set_t
{
	float	 *p_coefficients;
	size_t number_of_filter_coefficients ;
	size_t predefined_data_block_size ;
} fir_filter_api_set_params_t;

/**********  define API  functions  ************/


extern char fir_filter_module_name[] ;
#define FIR_FILTER_API_MODULE_NAME	fir_filter_module_name


#endif
