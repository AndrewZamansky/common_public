
#ifndef _dsp_management_internal_api_h_
#define _dsp_management_internal_api_h_

#include "_project_typedefs.h"
#include "_project_defines.h"

#define DSP_MAGIC_NUMBER        0xa2b6

enum DSP_PAD_TYPE_e {
	DSP_OUT_PAD_TYPE_NOT_USED = 0,
	DSP_OUT_PAD_TYPE_NORMAL,
	DSP_PAD_TYPE_DUMMY_ZERO_BUFFER,
	DSP_PAD_TYPE_DUMMY_OUTPUT_BUFFER,
	DSP_PAD_TYPE_CHAIN_INPUT_BUFFER,
	DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER,
};


typedef uint8_t (*dsp_ioctl_func_t)(struct dsp_module_inst_t *dsp,
		 uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)  ;
typedef void (*dsp_func_t)(struct dsp_module_inst_t *adsp);

struct dsp_module_type_t {
	const char        *name;
	dsp_ioctl_func_t  ioctl;
	dsp_func_t        dsp_func;
	uint16_t          module_data_size;
};


struct in_pads_t {
	struct dsp_pad_t  **in_pads_arr;
	uint8_t           max_used_input_num;
};

struct out_pads_t {
	struct dsp_pad_t  *out_pads_arr;
	uint8_t           max_used_output_num;
};

struct dsp_module_inst_t {
	struct dsp_module_type_t *module_type;
	void                     *handle;
	char const              *module_inst_name;
	dsp_management_api_module_control_t   ctl;
	struct in_pads_t       in_pads;
	struct out_pads_t      out_pads;
	uint16_t               magic_num;
};



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



void _dsp_register_new_module_type(const char *a_module_name,
		dsp_ioctl_func_t a_ioctle_func, dsp_func_t a_dsp_func,
		uint16_t a_module_data_size);

#define DSP_REGISTER_NEW_MODULE(name, ioctl_f, dsp_f, module_data_t)    \
	_dsp_register_new_module_type(name, ioctl_f, dsp_f, sizeof(module_data_t))




void dsp_get_output_buffer_from_pad( struct dsp_module_inst_t * adsp,
							uint8_t pad_num, real_t **buff, size_t *buff_len);
void dsp_get_input_buffer_from_pad( struct dsp_module_inst_t * adsp,
						uint8_t pad_num, real_t **buff, size_t *buff_len);

#endif
