
#ifndef _dsp_managment_api_h_
#define _dsp_managment_api_h_


//#include "src/_dsp_managment_prerequirements_check.h"// should be after dsp_managment_config.h

#define NOT_FOR_SAVE 	0
#define FOR_SAVE 		1

/**********  define API  types ************/





typedef struct
{
	uint8_t paramSetIoctl;
	uint8_t paramGetIoctl;
	uint8_t *paramStr;
	uint8_t usedForSave;
}dsp_param_t;





typedef uint8_t (*dsp_ioctl_func_t)(void * const aHandle ,
		const uint8_t aIoctl_num , void * aIoctl_param1 ,  void * aIoctl_param2)  ;
typedef uint8_t (*dsp_ioctl_0_params_func_t)(void * const aHandle ,const uint8_t aIoctl_num )  ;
typedef uint8_t (*dsp_ioctl_1_params_func_t)(void * const aHandle ,const uint8_t aIoctl_num , void *param1)  ;
typedef void (*dsp_func_t)(const void * const aHandle ,
		uint8_t num_of_inputs,uint8_t num_of_ouputs, size_t data_len ,
		float *apCh1In , float *apCh2In,
		float *apCh1Out , float *apCh2Out
		)  ;


typedef struct _dsp_descriptor_t
{
//	uint8_t 			name[DSP_CONFIG_MAX_DSP_NAME_LEN+1];// +1 for null char
	void*    			handle;
	dsp_ioctl_func_t  	ioctl;
	dsp_func_t  		dsp_func;

}dsp_descriptor_t,*pdsp_descriptor;

typedef const dsp_descriptor_t * pdsp_descriptor_const;



/**********  define API  functions  ************/


/*  ioctl functions */
#define DSP_IOCTL_0_PARAMS(dsp,ioctl_num)   			((dsp_ioctl_0_params_func_t)(dsp)->ioctl)((dsp)->handle,ioctl_num)
#define DSP_IOCTL_1_PARAMS(dsp,ioctl_num,ioctl_param)   ((dsp_ioctl_1_params_func_t)(dsp)->ioctl)((dsp)->handle,ioctl_num,ioctl_param)
#define DSP_IOCTL		DSP_IOCTL_1_PARAMS
#define DSP_IOCTL_2_PARAMS(dsp,ioctl_num,ioctl_param1,ioctl_param2)    (dsp)->ioctl((dsp)->handle,ioctl_num,ioctl_param1,ioctl_param2)



#define DSP_FUNC_1CH_IN_1CH_OUT(dsp,ch1In,ch1Out,len)    					(dsp)->dsp_func((dsp)->handle,1,1,len,ch1In,NULL,ch1Out,NULL)
#define DSP_FUNC_2CH_IN_2CH_OUT(dsp,ch1In,ch2In,ch1Out,ch2Out,len)    		(dsp)->dsp_func((dsp)->handle,2,2,len,ch1In,ch2In,ch1Out,ch2Out)


#endif
