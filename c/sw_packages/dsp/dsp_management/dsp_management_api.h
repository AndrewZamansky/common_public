
#ifndef _dsp_management_api_h_
#define _dsp_management_api_h_

#include "_project_typedefs.h"
#include "_project_defines.h"

#define MAX_NUM_OF_OUTPUT_PADS  4
#define DSP_MAGIC_NUMBER        0xa2b6

/**********  define API  types ************/

struct dsp_module_inst_t;

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


struct static_dsp_source_pad {
	const char *src_module_inst_name;
	uint8_t src_output_pad_num;
};

struct static_dsp_component {
	const char *module_inst_name;
	const char *module_type_name;
	struct static_dsp_source_pad source_pads[MAX_NUM_OF_OUTPUT_PADS];
};


typedef uint8_t (*dsp_ioctl_func_t)(struct dsp_module_inst_t *dsp,
		 uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)  ;
typedef void (*dsp_func_t)(struct dsp_module_inst_t *dsp,
		struct dsp_pad_t *in_pads[MAX_NUM_OF_OUTPUT_PADS],
		struct dsp_pad_t out_pads[MAX_NUM_OF_OUTPUT_PADS] )  ;

struct dsp_module_type_t {
	const char        *name;
	dsp_ioctl_func_t  ioctl;
	dsp_func_t        dsp_func;
	uint16_t          module_data_size;
};

 struct dsp_module_inst_t {
//	uint8_t         name[DSP_CONFIG_MAX_DSP_NAME_LEN+1];// +1 for null char
	void*                  handle;
	dsp_ioctl_func_t       ioctl;
	dsp_func_t             dsp_func;
	char const             *module_inst_name;
	dsp_management_api_module_control_t   ctl;
	struct dsp_pad_t        *in_pads[MAX_NUM_OF_OUTPUT_PADS];
	struct dsp_pad_t        out_pads[MAX_NUM_OF_OUTPUT_PADS];
	uint16_t                magic_num;
};

struct dsp_chain_t {
	struct dsp_module_inst_t *module_inst_arr;
	size_t            num_of_modules;

	struct dsp_pad_t  chain_in_pads[MAX_NUM_OF_OUTPUT_PADS];

	struct dsp_pad_t  *chain_out_pads[MAX_NUM_OF_OUTPUT_PADS];
	uint16_t          magic_num;
};

extern char chain_outputs_module_name[] ;
#define CHAIN_OUTPUTS_DSPT  chain_outputs_module_name
extern char chain_inputs_module_name[] ;
#define CHAIN_INPUTS_DSPT   chain_inputs_module_name

/**********  define API  functions  ************/

#ifdef __cplusplus
extern "C" {
#endif

/*  ioctl functions */
uint8_t	dsp_management_api_ioctl_0_params(
		struct dsp_chain_t *p_chain, char const*module_name, uint8_t ioctl_num);
uint8_t	dsp_management_api_ioctl_1_params(struct dsp_chain_t *p_chain,
		char const *module_name, uint8_t ioctl_num, void *param1);
uint8_t	dsp_management_api_ioctl_2_params(struct dsp_chain_t *p_chain,
		char const *module_name, uint8_t ioctl_num, void *param1, void *param2);


void dsp_management_api_set_module_control(
		struct dsp_chain_t *p_chain,
		char const*module_name,
		dsp_management_api_module_control_t ctl);


void dsp_management_api_delete_modules();



void dsp_management_api_set_chain_input_buffer(struct dsp_chain_t *ap_chain,
						DSP_INPUT_PADS_t sink_dsp_pad,
						uint8_t *buffer, size_t buff_size );
void dsp_management_api_set_chain_output_buffer(struct dsp_chain_t *ap_chain,
						DSP_OUTPUT_PADS_t output_dsp_pad,
						uint8_t *buffer, size_t buff_size);

void dsp_management_api_init(size_t max_num_of_allocated_buffers,
							size_t size_of_items_in_buffer);


extern struct dsp_chain_t *_dsp_management_create_static_chain(
		struct static_dsp_component arr[], uint32_t items_in_arr);
#define   DSP_MANAGEMENT_API_CREATE_STATIC_CHAIN(arr)  \
	_dsp_management_create_static_chain(arr, sizeof(arr)/sizeof(arr[0]))


void dsp_management_api_delete_chain(struct dsp_chain_t * ap_chain);


void dsp_management_api_process_chain(struct dsp_chain_t *ap_chain);

#ifdef __cplusplus
}
#endif


#endif
