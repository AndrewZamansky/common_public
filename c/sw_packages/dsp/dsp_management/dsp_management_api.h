
#ifndef _dsp_management_api_h_
#define _dsp_management_api_h_


//#include "src/_dsp_management_prerequirements_check.h"// should be after dsp_management_config.h

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
	DSP_MANAGEMENT_API_MODULE_CONTROL_ON  =0,
	DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS ,
	DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE ,
}DSP_MANAGEMENT_API_module_control_t;

typedef struct _dsp_out_pad_t
{
	float *buff;
	uint8_t pad_type;
	uint8_t total_registered_sinks;
	uint8_t sinks_processed_counter;
}dsp_pad_t;


typedef struct _dsp_descriptor_t	dsp_descriptor_t;
typedef dsp_descriptor_t*	pdsp_descriptor;



typedef uint8_t (*dsp_ioctl_func_t)(pdsp_descriptor dsp ,
		const uint8_t aIoctl_num , void * aIoctl_param1 ,  void * aIoctl_param2)  ;
typedef uint8_t (*dsp_ioctl_0_params_func_t)(pdsp_descriptor dsp ,const uint8_t aIoctl_num )  ;
typedef uint8_t (*dsp_ioctl_1_params_func_t)(pdsp_descriptor dsp ,const uint8_t aIoctl_num , void *param1)  ;
typedef void (*dsp_func_t)(pdsp_descriptor dsp , size_t data_len ,
		dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS] , dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS] )  ;

struct _dsp_descriptor_t
{
//	uint8_t 			name[DSP_CONFIG_MAX_DSP_NAME_LEN+1];// +1 for null char
	void*    			handle;
	dsp_ioctl_func_t  	ioctl;
	dsp_func_t  		dsp_func;
	DSP_MANAGEMENT_API_module_control_t				ctl;
	dsp_pad_t	    *in_pads[MAX_NUM_OF_OUTPUT_PADS];
	dsp_pad_t		out_pads[MAX_NUM_OF_OUTPUT_PADS];
};

typedef const dsp_descriptor_t * pdsp_descriptor_const;
typedef struct
{
	pdsp_descriptor* dsp_chain;
	size_t occupied_dsp_modules;
	size_t max_num_of_dsp_modules;
}dsp_chain_t;

/**********  define API  functions  ************/


/*  ioctl functions */
#define DSP_IOCTL_0_PARAMS(dsp,ioctl_num)   			((dsp_ioctl_0_params_func_t)(dsp)->ioctl)(dsp,ioctl_num)
#define DSP_IOCTL_1_PARAMS(dsp,ioctl_num,ioctl_param)   ((dsp_ioctl_1_params_func_t)(dsp)->ioctl)(dsp,ioctl_num,ioctl_param)
#define DSP_IOCTL		DSP_IOCTL_1_PARAMS
#define DSP_IOCTL_2_PARAMS(dsp,ioctl_num,ioctl_param1,ioctl_param2)    (dsp)->ioctl(dsp,ioctl_num,ioctl_param1,ioctl_param2)



//void DSP_FUNC_1CH_IN_1CH_OUT(pdsp_descriptor dsp,void *ch1In,size_t	len);
//void DSP_FUNC_2CH_IN_2CH_OUT(pdsp_descriptor dsp,void *ch1In,void *ch2In,size_t	len);
//void DSP_FUNC_2CH_IN_1CH_OUT(pdsp_descriptor dsp,void *ch1In,void *ch2In,size_t	len);
//void DSP_FUNC_2CH_IN_1CH_OUT_NO_OUTPUT_ALLOCATION(pdsp_descriptor dsp,void *ch1In,void *ch2In,void *ch1Out,size_t	len);
//void DSP_FUNC_1CH_IN_2CH_OUT(pdsp_descriptor dsp,void *ch1In,size_t	len);

void dsp_management_api_set_buffers_pool(void *adsp_buffers_pool);
void dsp_management_api_set_module_control(pdsp_descriptor dsp , DSP_MANAGEMENT_API_module_control_t ctl);

uint8_t DSP_CREATE_LINK(pdsp_descriptor source_dsp,DSP_OUTPUT_PADS_t source_dsp_pad,
		pdsp_descriptor sink_dsp,DSP_INPUT_PADS_t sink_dsp_pad);

void DSP_SET_SOURCE_BUFFER(pdsp_descriptor source_dsp,DSP_INPUT_PADS_t source_dsp_pad, void *buffer);
void DSP_SET_SINK_BUFFER(pdsp_descriptor dsp,DSP_OUTPUT_PADS_t dsp_output_pad, void *buffer);

dsp_chain_t *DSP_CREATE_CHAIN(size_t max_num_of_dsp_modules);
void DSP_ADD_MODULE_TO_CHAIN(dsp_chain_t *ap_chain, pdsp_descriptor dsp_module);

void DSP_PROCESS(pdsp_descriptor dsp , size_t	len);
void DSP_PROCESS_CHAIN(dsp_chain_t *ap_chain , size_t	len );

#endif