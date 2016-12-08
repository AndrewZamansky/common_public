
#ifndef _dsp_management_api_h_
#define _dsp_management_api_h_

/* check one two */
//#include "src/_dsp_management_prerequirements_check.h"// should be after dsp_management_config.h

#define NOT_FOR_SAVE 	0
#define FOR_SAVE 		1


#define	MAX_NUM_OF_OUTPUT_PADS	3


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
	IOCTL_DSP_INIT,
	IOCTL_DSP_LAST_COMMON_IOCTL
}common_dsp_ioctl_t;




typedef enum
{
	DSP_OUTPUT_PAD_0  =0,
	DSP_OUTPUT_PAD_1,
	DSP_OUTPUT_PAD_2
}DSP_OUTPUT_PADS_t;

typedef enum
{
	DSP_INPUT_PAD_0  =0,
	DSP_INPUT_PAD_1,
	DSP_INPUT_PAD_2
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
}dsp_management_api_module_control_t;

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

typedef struct
{
	char  				*name;
	dsp_ioctl_func_t  	ioctl;
	dsp_func_t  		dsp_func;
	uint16_t  			module_data_size;
} dsp_module_t;

struct _dsp_descriptor_t
{
//	uint8_t 			name[DSP_CONFIG_MAX_DSP_NAME_LEN+1];// +1 for null char
	void*    			handle;
	dsp_ioctl_func_t  	ioctl;
	dsp_func_t  		dsp_func;
	dsp_management_api_module_control_t				ctl;
	dsp_pad_t	    	*in_pads[MAX_NUM_OF_OUTPUT_PADS];
	dsp_pad_t			out_pads[MAX_NUM_OF_OUTPUT_PADS];
};

typedef const dsp_descriptor_t * pdsp_descriptor_const;
typedef struct
{
	pdsp_descriptor* 	dsp_chain;
	size_t 				occupied_dsp_modules;
	size_t 				max_num_of_dsp_modules;
	dsp_pad_t	    	chain_in_pads[MAX_NUM_OF_OUTPUT_PADS + 1];// +1 for dummy zero's input for unconnected inputs of modules
	dsp_pad_t*			chain_out_pads[MAX_NUM_OF_OUTPUT_PADS];
}dsp_chain_t;

/**********  define API  functions  ************/


/*  ioctl functions */
#define DSP_IOCTL_0_PARAMS(dsp,ioctl_num)   			((dsp_ioctl_0_params_func_t)(dsp)->ioctl)(dsp,ioctl_num)
#define DSP_IOCTL_1_PARAMS(dsp,ioctl_num,ioctl_param)   ((dsp_ioctl_1_params_func_t)(dsp)->ioctl)(dsp,ioctl_num,(void*)ioctl_param)
#define DSP_IOCTL		DSP_IOCTL_1_PARAMS
#define DSP_IOCTL_2_PARAMS(dsp,ioctl_num,ioctl_param1,ioctl_param2)    (dsp)->ioctl(dsp,ioctl_num,(void*)ioctl_param1,(void*)ioctl_param2)





void dsp_management_api_set_module_control(pdsp_descriptor dsp , dsp_management_api_module_control_t ctl);

void _DSP_REGISTER_NEW_MODULE(char *a_module_name, dsp_ioctl_func_t a_ioctle_func
		, dsp_func_t a_dsp_func , uint16_t a_module_data_size);
#define DSP_REGISTER_NEW_MODULE(a_module_name, a_ioctle_func , a_dsp_func , a_module_data_size)	\
		_DSP_REGISTER_NEW_MODULE(a_module_name, a_ioctle_func , a_dsp_func , sizeof(a_module_data_size))

uint8_t DSP_CREATE_INTER_MODULES_LINK(pdsp_descriptor source_dsp,DSP_OUTPUT_PADS_t source_dsp_pad,
		pdsp_descriptor sink_dsp,DSP_INPUT_PADS_t sink_dsp_pad);

uint8_t DSP_CREATE_CHAIN_INPUT_TO_MODULE_LINK(dsp_chain_t *ap_chain,DSP_INPUT_PADS_t source_dsp_pad,
		pdsp_descriptor sink_dsp,DSP_INPUT_PADS_t sink_dsp_pad);

uint8_t DSP_CREATE_MODULE_TO_CHAIN_OUTPUT_LINK(dsp_chain_t *ap_chain,DSP_OUTPUT_PADS_t sink_dsp_pad,
		pdsp_descriptor source_dsp,DSP_OUTPUT_PADS_t source_dsp_pad);

void DSP_SET_CHAIN_INPUT_BUFFER(dsp_chain_t *ap_chain,DSP_INPUT_PADS_t sink_dsp_pad, void *buffer);
void DSP_SET_CHAIN_OUTPUT_BUFFER(dsp_chain_t *ap_chain,DSP_OUTPUT_PADS_t source_dsp_pad, void *buffer);

dsp_chain_t *DSP_CREATE_CHAIN(size_t max_num_of_dsp_modules , void *adsp_buffers_pool);
void DSP_ADD_MODULE_TO_CHAIN(dsp_chain_t *ap_chain, char *a_module_name,  pdsp_descriptor dsp_module);

void DSP_PROCESS(pdsp_descriptor dsp , size_t	len);
void DSP_PROCESS_CHAIN(dsp_chain_t *ap_chain , size_t	len );

#endif
