
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






typedef enum
{
	DSP_OUTPUT_PAD_0  =0,
	DSP_OUTPUT_PAD_1
}DSP_OUTPUT_PADS_t;

typedef enum
{
	DSP_INPUT_PAD_0  =0,
	DSP_INPUT_PAD_1
}DSP_INPUT_PADS_t;

typedef enum
{
	DSP_PAD_TYPE_NOT_USED  = 0,
	DSP_PAD_TYPE_NORMAL   ,
	DSP_PAD_TYPE_NOT_ALLOCATED_BUFFER ,
}DSP_PAD_TYPE_t;

typedef enum
{
	DSP_MANAGMENT_API_MODULE_CONTROL_ON  =0,
	DSP_MANAGMENT_API_MODULE_CONTROL_BYPASS ,
	DSP_MANAGMENT_API_MODULE_CONTROL_MUTE ,
}DSP_MANAGMENT_API_module_control_t;

typedef struct _dsp_out_pad_t
{
	float *buff;
	uint8_t pad_type;
	uint8_t total_registered_sinks;
	uint8_t sinks_processed_counter;
}dsp_pad_t;




typedef uint8_t (*dsp_ioctl_func_t)(void * const aHandle ,
		const uint8_t aIoctl_num , void * aIoctl_param1 ,  void * aIoctl_param2)  ;
typedef uint8_t (*dsp_ioctl_0_params_func_t)(void * const aHandle ,const uint8_t aIoctl_num )  ;
typedef uint8_t (*dsp_ioctl_1_params_func_t)(void * const aHandle ,const uint8_t aIoctl_num , void *param1)  ;
typedef void (*dsp_func_t)(const void * const aHandle , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS] )  ;

typedef struct _dsp_descriptor_t
{
//	uint8_t 			name[DSP_CONFIG_MAX_DSP_NAME_LEN+1];// +1 for null char
	void*    			handle;
	dsp_ioctl_func_t  	ioctl;
	dsp_func_t  		dsp_func;
	DSP_MANAGMENT_API_module_control_t				ctl;
	dsp_pad_t	    *in_pads[MAX_NUM_OF_OUTPUT_PADS];
	dsp_pad_t		out_pads[MAX_NUM_OF_OUTPUT_PADS];
}dsp_descriptor_t,*pdsp_descriptor;

typedef const dsp_descriptor_t * pdsp_descriptor_const;

/**********  define API  functions  ************/


/*  ioctl functions */
#define DSP_IOCTL_0_PARAMS(dsp,ioctl_num)   			((dsp_ioctl_0_params_func_t)(dsp)->ioctl)((dsp)->handle,ioctl_num)
#define DSP_IOCTL_1_PARAMS(dsp,ioctl_num,ioctl_param)   ((dsp_ioctl_1_params_func_t)(dsp)->ioctl)((dsp)->handle,ioctl_num,ioctl_param)
#define DSP_IOCTL		DSP_IOCTL_1_PARAMS
#define DSP_IOCTL_2_PARAMS(dsp,ioctl_num,ioctl_param1,ioctl_param2)    (dsp)->ioctl((dsp)->handle,ioctl_num,ioctl_param1,ioctl_param2)



//void DSP_FUNC_1CH_IN_1CH_OUT(pdsp_descriptor dsp,void *ch1In,size_t	len);
//void DSP_FUNC_2CH_IN_2CH_OUT(pdsp_descriptor dsp,void *ch1In,void *ch2In,size_t	len);
//void DSP_FUNC_2CH_IN_1CH_OUT(pdsp_descriptor dsp,void *ch1In,void *ch2In,size_t	len);
//void DSP_FUNC_2CH_IN_1CH_OUT_NO_OUTPUT_ALLOCATION(pdsp_descriptor dsp,void *ch1In,void *ch2In,void *ch1Out,size_t	len);
//void DSP_FUNC_1CH_IN_2CH_OUT(pdsp_descriptor dsp,void *ch1In,size_t	len);

void dsp_managment_api_set_buffers_pool(void *adsp_buffers_pool);
void dsp_managment_api_set_module_control(pdsp_descriptor dsp , DSP_MANAGMENT_API_module_control_t ctl);

uint8_t DSP_CREATE_LINK(pdsp_descriptor source_dsp,DSP_OUTPUT_PADS_t source_dsp_pad,
		pdsp_descriptor sink_dsp,DSP_INPUT_PADS_t sink_dsp_pad);

void DSP_SET_SOURCE_BUFFER(pdsp_descriptor source_dsp,DSP_INPUT_PADS_t source_dsp_pad, void *buffer);
void DSP_SET_SINK_BUFFER(pdsp_descriptor dsp,DSP_INPUT_PADS_t dsp_output_pad, void *buffer);

void DSP_PROCESS(pdsp_descriptor dsp , size_t	len);

#endif
