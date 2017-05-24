
#ifndef _downsampling_by_int_filter_API_H_
#define _downsampling_by_int_filter_API_H_

#include "dsp_management_api.h"


/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  types ************/

typedef enum
{
	IOCTL_DOWNSAMPLING_BY_INT_SET_FACTOR = IOCTL_DSP_LAST_COMMON_IOCTL + 1 ,
	IOCTL_DOWNSAMPLING_BY_INT_SET_FIR_COEFFICIENTS
}DOWNSAMPLING_BY_INT_FILTER_API_ioctl_t;

typedef struct _downsampling_by_int_filter_api_band_set_t
{
	float	 *p_coefficients;
	size_t number_of_filter_coefficients ;
	size_t predefined_data_block_size ;
} downsampling_by_int_api_set_params_t;

/**********  define API  functions  ************/


extern char downsampling_by_int_module_name[] ;
#define DOWNSAMPLING_BY_INT_API_MODULE_NAME	downsampling_by_int_module_name


#endif
