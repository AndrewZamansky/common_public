
#ifndef _dsp_management_api_h_
#define _dsp_management_api_h_

#include "_project_typedefs.h"
#include "_project_defines.h"


#define DSP_CHAIN_CREATION_API_VER    20190924

struct dsp_module_inst_t;


enum common_dsp_ioctl_e {
	IOCTL_DSP_INIT = 0,
	IOCTL_DSP_DELETE,
	IOCTL_DSP_LAST_COMMON_IOCTL
};


#define IN_PAD(num)   (num)
#define OUT_PAD(num)  (num)


typedef enum dsp_management_api_module_control_e {
	DSP_MANAGEMENT_API_MODULE_CONTROL_ON = 0,
	DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS,
	DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE,
} dsp_management_api_module_control_t;


#define MAX_DSP_INPUT_PAD   0xff
#define LAST_INPUT_PAD_DESC {NULL, 0, MAX_DSP_INPUT_PAD}
#define IN0(source_module, out_pad_num)  {source_module, out_pad_num, 0}
#define IN1(source_module, out_pad_num)  {source_module, out_pad_num, 1}
#define IN2(source_module, out_pad_num)  {source_module, out_pad_num, 2}
#define IN3(source_module, out_pad_num)  {source_module, out_pad_num, 3}
#define IN4(source_module, out_pad_num)  {source_module, out_pad_num, 4}
#define IN5(source_module, out_pad_num)  {source_module, out_pad_num, 5}
#define IN6(source_module, out_pad_num)  {source_module, out_pad_num, 6}
#define IN7(source_module, out_pad_num)  {source_module, out_pad_num, 7}
#define IN8(source_module, out_pad_num)  {source_module, out_pad_num, 8}
#define IN9(source_module, out_pad_num)  {source_module, out_pad_num, 9}
#define IN10(source_module, out_pad_num)  {source_module, out_pad_num, 10}
#define IN11(source_module, out_pad_num)  {source_module, out_pad_num, 11}
#define IN12(source_module, out_pad_num)  {source_module, out_pad_num, 12}
#define IN13(source_module, out_pad_num)  {source_module, out_pad_num, 13}
#define IN14(source_module, out_pad_num)  {source_module, out_pad_num, 14}
#define IN15(source_module, out_pad_num)  {source_module, out_pad_num, 15}
#define SET_INPUTS(...) (struct static_dsp_input_pad_desc_t[]) \
									{ __VA_ARGS__, LAST_INPUT_PAD_DESC}
#define NO_INPUTS  LAST_INPUT_PAD_DESC

struct static_dsp_input_pad_desc_t {
	const char *src_module_inst_name;
	uint8_t src_output_pad_num;
	uint8_t input_pad_num;
};


struct static_dsp_component_t {
	const char *module_inst_name;
	const char *module_type_name;
	struct static_dsp_input_pad_desc_t *input_pads_desc;
};



typedef void* chain_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

extern char chain_outputs_module_name[] ;
#define CHAIN_OUTPUTS_DSPT  chain_outputs_module_name
extern char chain_inputs_module_name[] ;
#define CHAIN_INPUTS_DSPT   chain_inputs_module_name



/*  ioctl functions */
uint8_t	dsp_management_api_ioctl_0_params(chain_handle_t chain_handle,
		char const*module_name, uint8_t ioctl_num);
uint8_t	dsp_management_api_ioctl_1_params(chain_handle_t chain_handle,
		char const *module_name, uint8_t ioctl_num, void *param1);
uint8_t	dsp_management_api_ioctl_2_params(chain_handle_t chain_handle,
		char const *module_name, uint8_t ioctl_num, void *param1, void *param2);


void dsp_management_api_set_module_control( chain_handle_t chain_handle,
			char const*module_name, dsp_management_api_module_control_t ctl);


void dsp_management_api_delete_modules(void);



void dsp_management_api_set_chain_input_buffer(chain_handle_t chain_handle,
			uint16_t chain_input_pad_num, uint8_t *buffer, size_t buff_size );
void dsp_management_api_set_chain_output_buffer(chain_handle_t chain_handle,
			uint16_t chain_output_pad_num, uint8_t *buffer, size_t buff_size);

void dsp_management_api_init(size_t size_of_items_in_buffer);
void dsp_management_api_delete(void);


extern chain_handle_t _dsp_management_create_static_chain(
		struct static_dsp_component_t arr[], uint32_t items_in_arr);
#define   DSP_MANAGEMENT_API_CREATE_STATIC_CHAIN(arr)  \
	_dsp_management_create_static_chain(arr, sizeof(arr)/sizeof(arr[0]))


void dsp_management_api_delete_chain(chain_handle_t chain_handle);


void dsp_management_api_process_chain(chain_handle_t chain_handle);

#ifdef __cplusplus
}
#endif


#endif
