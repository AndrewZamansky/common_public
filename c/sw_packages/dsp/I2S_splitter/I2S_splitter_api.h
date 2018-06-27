
#ifndef _I2S_splitter_API_H_
#define _I2S_splitter_API_H_


/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  functions  ************/



typedef enum
{
	IOCTL_I2S_SPLITTER_SET_NUM_OF_CHANNELS = IOCTL_DSP_LAST_COMMON_IOCTL+1,
}I2S_SPLITTER_API_ioctl_t;


extern char I2S_splitter_module_name[] ;
#define I2S_SPLITTER_DSPT	I2S_splitter_module_name

#endif
