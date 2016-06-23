
#ifndef _I2S_NUC505_API_H_
#define _I2S_NUC505_API_H_

#include "src/_I2S_nuc505_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/

typedef enum
{
	I2S_SET_VOLUME_IOCTL = IOCTL_LAST_COMMON_IOCTL+1,
	I2S_SET_PARAMS,
	I2S_ENABLE_OUTPUT_IOCTL
}I2S_API_ioctl_t;


typedef struct
{
	uint16_t num_of_words_in_buffer_per_chenel;
	uint8_t	num_of_bytes_in_word;
}I2S_API_set_params_t;

/**********  define API  functions  ************/

uint8_t  I2S_nuc505_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#include "src/I2S_nuc505_static_dev_macros.h"


#define I2S_NUC505_API_CREATE_STATIC_DEV( pdev , callback_pdev ) \
			__I2S_NUC505_API_CREATE_STATIC_DEV( pdev , callback_pdev )


#endif
