
#ifndef _I2S_splitter_API_H_
#define _I2S_splitter_API_H_

#include "I2S_splitter_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "src/_I2S_splitter_prerequirements_check.h" // should be after {I2S_splitter_config.h,dev_managment_api.h}

#include "common_dsp_api.h"

/*****************  defines  **************/


/**********  define API  types ************/






/**********  define API  functions  ************/



uint8_t  I2S_splitter_api_init_dsp_descriptor(pdsp_descriptor aDspDescriptor);

//#include "src/_I2S_splitter_static_dev_macros.h"
//
//#define I2S_SPLITTER_API_CREATE_STATIC_DEV(dev,dev_name)  __I2S_SPLITTER_API_CREATE_STATIC_DEV(dev,dev_name )
//

#endif
