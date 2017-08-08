
#ifndef _dsp_management_api_h_
#define _dsp_management_api_h_

#include "_project_typedefs.h"
#include "_project_defines.h"

#define	MAX_NUM_OF_OUTPUT_PADS	3


/**********  define API  types ************/

struct dsp_desc_t;

#if defined(CONFIG_DSP_REAL_NUMBER_FORMAT_FLOATING_POINT)
	#define real_t    float
#elif defined(CONFIG_DSP_REAL_NUMBER_FORMAT_FIXED_POINT)
	#ifdef __cplusplus
		#include "fix16.hpp"
	#endif
	#define real_t    Fix16
#else
	#error "undefined real numbers format"
#endif



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
	DSP_PAD_TYPE_DUMMY_ZERO_BUFFER ,
	DSP_PAD_TYPE_CHAIN_INPUT_BUFFER ,
	DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER ,
}DSP_PAD_TYPE_t;

typedef enum
{
	DSP_MANAGEMENT_API_MODULE_CONTROL_ON  =0,
	DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS ,
	DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE ,
}dsp_management_api_module_control_t;

struct dsp_pad_t {
#ifdef __cplusplus
	real_t *buff;
#else
	void *buff;
#endif
	size_t buff_size;
	uint8_t pad_type;
	uint8_t total_registered_sinks;
	uint8_t sinks_processed_counter;
};



typedef uint8_t (*dsp_ioctl_func_t)(struct dsp_desc_t *dsp,
		 uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)  ;
typedef void (*dsp_func_t)(struct dsp_desc_t *dsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS] )  ;

struct dsp_module_t {
	const char        *name;
	dsp_ioctl_func_t  ioctl;
	dsp_func_t        dsp_func;
	uint16_t          module_data_size;
};

 struct dsp_desc_t {
//	uint8_t         name[DSP_CONFIG_MAX_DSP_NAME_LEN+1];// +1 for null char
	void*                  handle;
	dsp_ioctl_func_t       ioctl;
	dsp_func_t             dsp_func;
	dsp_management_api_module_control_t   ctl;
	struct dsp_pad_t        *in_pads[MAX_NUM_OF_OUTPUT_PADS];
	struct dsp_pad_t        out_pads[MAX_NUM_OF_OUTPUT_PADS];
};

struct dsp_chain_t {
	struct dsp_desc_t **dsp_chain;
	size_t            occupied_dsp_modules;
	size_t            max_num_of_dsp_modules;

	struct dsp_pad_t  chain_in_pads[MAX_NUM_OF_OUTPUT_PADS];

	struct dsp_pad_t  *chain_out_pads[MAX_NUM_OF_OUTPUT_PADS];
	//real_t            *out_buffers[MAX_NUM_OF_OUTPUT_PADS];
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

#ifdef __cplusplus
extern "C" {
#endif

void dsp_management_api_set_module_control(struct dsp_desc_t *dsp,
								dsp_management_api_module_control_t ctl);


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
						DSP_INPUT_PADS_t sink_dsp_pad,
						uint8_t *buffer, size_t buff_size );
void DSP_SET_CHAIN_OUTPUT_BUFFER(struct dsp_chain_t *ap_chain,
						DSP_OUTPUT_PADS_t output_dsp_pad,
						uint8_t *buffer, size_t buff_size);


struct dsp_chain_t *DSP_CREATE_CHAIN(size_t max_num_of_dsp_modules ,
									size_t max_num_of_allocated_buffers);
void DSP_DELETE_CHAIN(struct dsp_chain_t * ap_chain);
void DSP_ADD_MODULE_TO_CHAIN(struct dsp_chain_t *ap_chain,
		char *a_module_name,  struct dsp_desc_t *dsp_module);


void DSP_PROCESS_CHAIN(struct dsp_chain_t *ap_chain);

#ifdef __cplusplus
}
#endif


#endif
