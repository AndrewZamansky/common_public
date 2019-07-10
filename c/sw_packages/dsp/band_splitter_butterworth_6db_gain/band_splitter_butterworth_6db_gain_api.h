
#ifndef _band_splitter_butterworth_6db_gain_API_H_
#define _band_splitter_butterworth_6db_gain_API_H_

#include "dsp_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	IOCTL_BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_SET_FC =
								IOCTL_DSP_LAST_COMMON_IOCTL + 1 ,
	IOCTL_BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_GET_PARAMS
}BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_API_ioctl_t;


struct band_splitter_butterworth_6db_gain_api_set_params_t
{
	float fc;
};


/**********  define API  functions  ************/

#ifdef __cplusplus
extern "C" {
#endif

extern char band_splitter_butterworth_6db_gain_module_name[] ;
#define BAND_SPLITTER_BUTTERWORTH_6DB_GAIN_DSPT	\
							band_splitter_butterworth_6db_gain_module_name

#ifdef __cplusplus
}
#endif

#endif
