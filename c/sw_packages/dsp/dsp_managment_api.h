
#ifndef _dsp_managment_api_h_
#define _dsp_managment_api_h_


//#include "src/_dsp_managment_prerequirements_check.h"// should be after dsp_managment_config.h

#define NOT_FOR_SAVE 	0
#define FOR_SAVE 		1


#define	MAX_NUM_OF_OUTPUT_PADS	2


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
typedef void (*dsp_func_t)(const void * const aHandle , size_t data_len ,
		float *in_pads[MAX_NUM_OF_OUTPUT_PADS] , float *out_pads[MAX_NUM_OF_OUTPUT_PADS] )  ;


typedef struct _dsp_descriptor_t
{
//	uint8_t 			name[DSP_CONFIG_MAX_DSP_NAME_LEN+1];// +1 for null char
	void*    			handle;
	dsp_ioctl_func_t  	ioctl;
	dsp_func_t  		dsp_func;
	float				*in_pads[MAX_NUM_OF_OUTPUT_PADS];
	float				*out_pads[MAX_NUM_OF_OUTPUT_PADS];
}dsp_descriptor_t,*pdsp_descriptor;

typedef const dsp_descriptor_t * pdsp_descriptor_const;



/**********  define API  functions  ************/


/*  ioctl functions */
#define DSP_IOCTL_0_PARAMS(dsp,ioctl_num)   			((dsp_ioctl_0_params_func_t)(dsp)->ioctl)((dsp)->handle,ioctl_num)
#define DSP_IOCTL_1_PARAMS(dsp,ioctl_num,ioctl_param)   ((dsp_ioctl_1_params_func_t)(dsp)->ioctl)((dsp)->handle,ioctl_num,ioctl_param)
#define DSP_IOCTL		DSP_IOCTL_1_PARAMS
#define DSP_IOCTL_2_PARAMS(dsp,ioctl_num,ioctl_param1,ioctl_param2)    (dsp)->ioctl((dsp)->handle,ioctl_num,ioctl_param1,ioctl_param2)


extern	void *dsp_buffers_pool;

#define DSP_FUNC_1CH_IN_1CH_OUT(dsp,ch1In,len)    					\
		(dsp)->in_pads[0]	= (float*)ch1In;	\
		(dsp)->out_pads[0]	= (float*)memory_pool_malloc(dsp_buffers_pool);	\
		(dsp)->dsp_func((dsp)->handle,len,(dsp)->in_pads,(dsp)->out_pads )

#define DSP_FUNC_2CH_IN_2CH_OUT(dsp,ch1In,ch2In,len)    		\
		(dsp)->in_pads[0]	= (float*)ch1In;	\
		(dsp)->in_pads[1]	= (float*)ch2In;	\
		(dsp)->out_pads[0]	= (float*)memory_pool_malloc(dsp_buffers_pool);	\
		(dsp)->out_pads[1]	= (float*)memory_pool_malloc(dsp_buffers_pool);	\
		(dsp)->dsp_func((dsp)->handle,len,(dsp)->in_pads,(dsp)->out_pads )

#define DSP_FUNC_2CH_IN_1CH_OUT(dsp,ch1In,ch2In,len)    		\
		(dsp)->in_pads[0]	= (float*)ch1In;	\
		(dsp)->in_pads[1]	= (float*)ch2In;	\
		(dsp)->out_pads[0]	= (float*)memory_pool_malloc(dsp_buffers_pool);	\
		(dsp)->dsp_func((dsp)->handle,len,(dsp)->in_pads,(dsp)->out_pads )

#define DSP_FUNC_2CH_IN_1CH_OUT_NO_OUTPUT_ALLOCATION(dsp,ch1In,ch2In,ch1Out,len)    		\
		(dsp)->in_pads[0]	= (float*)ch1In;	\
		(dsp)->in_pads[1]	= (float*)ch2In;	\
		(dsp)->out_pads[0]	= (float*)ch1Out;	\
		(dsp)->dsp_func((dsp)->handle,len,(dsp)->in_pads,(dsp)->out_pads )

#define DSP_FUNC_1CH_IN_2CH_OUT(dsp,ch1In,len)    		\
		(dsp)->in_pads[0]	= (float*)ch1In;	\
		(dsp)->out_pads[0]	= (float*)memory_pool_malloc(dsp_buffers_pool);	\
		(dsp)->out_pads[1]	= (float*)memory_pool_malloc(dsp_buffers_pool);	\
		(dsp)->dsp_func((dsp)->handle,len,(dsp)->in_pads,(dsp)->out_pads )


#endif
