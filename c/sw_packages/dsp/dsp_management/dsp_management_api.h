
#ifndef _dsp_management_api_h_
#define _dsp_management_api_h_

#include "_project_typedefs.h"
#include "_project_defines.h"

#define	MAX_NUM_OF_OUTPUT_PADS	3


/**********  define API  types ************/
struct dsp_desc_t;


typedef enum
{
	IOCTL_DSP_INIT,
	IOCTL_DSP_DELETE,
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
	DSP_OUT_PAD_TYPE_NOT_USED  = 0,
	DSP_OUT_PAD_TYPE_NORMAL   ,
	DSP_OUT_PAD_TYPE_NOT_ALLOCATED_BUFFER ,
}DSP_PAD_TYPE_t;

typedef enum
{
	DSP_MANAGEMENT_API_MODULE_CONTROL_ON  =0,
	DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS ,
	DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE ,
}dsp_management_api_module_control_t;

struct dsp_pad_t {
	float *buff;
	uint8_t pad_type;
	uint8_t total_registered_sinks;
	uint8_t sinks_processed_counter;
};



typedef uint8_t (*dsp_ioctl_func_t)(struct dsp_desc_t *dsp,
		 uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)  ;
typedef void (*dsp_func_t)(struct dsp_desc_t *dsp, size_t data_len ,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS] )  ;

struct dsp_module_t {
	char  				*name;
	dsp_ioctl_func_t  	ioctl;
	dsp_func_t  		dsp_func;
	uint16_t  			module_data_size;
};

 struct dsp_desc_t {
//	uint8_t 			name[DSP_CONFIG_MAX_DSP_NAME_LEN+1];// +1 for null char
	void*    			handle;
	dsp_ioctl_func_t  	ioctl;
	dsp_func_t  		dsp_func;
	dsp_management_api_module_control_t				ctl;
	struct dsp_pad_t	    	*in_pads[MAX_NUM_OF_OUTPUT_PADS];
	struct dsp_pad_t			out_pads[MAX_NUM_OF_OUTPUT_PADS];
};

struct dsp_chain_t {
	struct dsp_desc_t **dsp_chain;
	size_t 				occupied_dsp_modules;
	size_t 				max_num_of_dsp_modules;

	struct dsp_pad_t	chain_in_pads[MAX_NUM_OF_OUTPUT_PADS];

	struct dsp_pad_t*	chain_out_pads[MAX_NUM_OF_OUTPUT_PADS];
	float *				out_buffers[MAX_NUM_OF_OUTPUT_PADS];
};

/**********  define API  functions  ************/


/*  ioctl functions */
#define DSP_IOCTL_0_PARAMS(dsp, ioctl_num)   		\
							((dsp)->ioctl)(dsp, ioctl_num, NULL, NULL)
#define DSP_IOCTL_1_PARAMS(dsp, ioctl_num, ioctl_param)   \
							((dsp)->ioctl)(dsp, ioctl_num, ioctl_param, NULL)
#define DSP_IOCTL		DSP_IOCTL_1_PARAMS
#define DSP_IOCTL_2_PARAMS(dsp,ioctl_num,ioctl_param1,ioctl_param2)   \
					((dsp)->ioctl)(dsp, ioctl_num, ioctl_param1, ioctl_param2)





void dsp_management_api_set_module_control(struct dsp_desc_t *dsp,
								dsp_management_api_module_control_t ctl);

void _DSP_REGISTER_NEW_MODULE(char *a_module_name,
		dsp_ioctl_func_t a_ioctle_func, dsp_func_t a_dsp_func,
		uint16_t a_module_data_size);

#define DSP_REGISTER_NEW_MODULE(name, ioctl_f, dsp_f, module_data_t)	\
		_DSP_REGISTER_NEW_MODULE(name, ioctl_f, dsp_f, sizeof(module_data_t))

void DSP_DELETE_MODULES();

uint8_t DSP_CREATE_INTER_MODULES_LINK(struct dsp_desc_t *src_dsp,
		DSP_OUTPUT_PADS_t src_dsp_pad, struct dsp_desc_t *sink_dsp,
		DSP_INPUT_PADS_t sink_dsp_pad);

uint8_t DSP_CREATE_CHAIN_INPUT_TO_MODULE_LINK(struct dsp_chain_t *ap_chain,
		DSP_INPUT_PADS_t src_dsp_pad, struct dsp_desc_t *sink_dsp,
		DSP_INPUT_PADS_t sink_dsp_pad);

uint8_t DSP_CREATE_MODULE_TO_CHAIN_OUTPUT_LINK(struct dsp_chain_t *ap_chain,
		DSP_OUTPUT_PADS_t sink_dsp_pad, struct dsp_desc_t *src_dsp,
		DSP_OUTPUT_PADS_t src_dsp_pad);

void DSP_SET_CHAIN_INPUT_BUFFER(struct dsp_chain_t *ap_chain,
				DSP_INPUT_PADS_t sink_dsp_pad, void *buffer);
void DSP_SET_CHAIN_OUTPUT_BUFFER(struct dsp_chain_t *ap_chain,
				DSP_OUTPUT_PADS_t src_dsp_pad, void *buffer);

struct dsp_chain_t *DSP_CREATE_CHAIN(size_t max_num_of_dsp_modules ,
											void *adsp_buffers_pool);
void DSP_DELETE_CHAIN(struct dsp_chain_t * ap_chain);
void DSP_ADD_MODULE_TO_CHAIN(struct dsp_chain_t *ap_chain,
		char *a_module_name,  struct dsp_desc_t *dsp_module);

void DSP_PROCESS_CHAIN(struct dsp_chain_t *ap_chain , size_t	len );

#endif
