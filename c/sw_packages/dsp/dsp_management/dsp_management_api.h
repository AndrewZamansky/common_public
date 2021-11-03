
#ifndef _dsp_management_api_h_
#define _dsp_management_api_h_

#include "_project_typedefs.h"
#include "_project_defines.h"


#define DSP_CHAIN_CREATION_API_VER    20201001

struct dsp_module_inst_t;


enum common_dsp_ioctl_e {
	IOCTL_DSP_INIT = 0,
	IOCTL_DSP_DELETE,
	IOCTL_DSP_LAST_COMMON_IOCTL
};


#define IN_PAD(num)   (num)
#define OUT_PAD(num)  (num)


enum dsp_management_api_module_control_e {
	DSP_MANAGEMENT_API_MODULE_CONTROL_ON = 0,
	DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS,
	DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE,
};


#define MAX_DSP_INPUT_PAD   0xff
#define LAST_INPUT_PAD_DESC  {NULL, 0, MAX_DSP_INPUT_PAD}
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


/* Following macros are used to create DSP_IOCTL()*/
#define DSP_PP_ARG_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
         _61, _62, _63, N, ...) N

#define DSP_PP_RSEQ_N()                                        \
         62, 61, 60,                                       \
         59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
         49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
         39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
         19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
          9,  8,  7,  6,  5,  4,  3,  2,  1,  0 , 0

#define DSP_PP_NARG_(...)    DSP_PP_ARG_N(__VA_ARGS__)
#define DSP_PP_NARG(...)     DSP_PP_NARG_(__VA_ARGS__, DSP_PP_RSEQ_N())

/* ioctl functions */
uint8_t _dsp_ioctl_2_params(chain_handle_t chain_handle,
		char const *module_name, uint8_t ioctl_num, void *param1, void *param2);
#define _ioctl_1_params(chain, dsp, ioctl_num, param1) \
				_dsp_ioctl_2_params(chain, dsp, ioctl_num, param1, NULL)
#define _ioctl_0_params(chain, dsp, ioctl_num) \
				_dsp_ioctl_2_params(chain, dsp, ioctl_num, NULL, NULL)
#define _DSP_IOCTL_STEP2(a, chain, dsp, ...)  \
					_ioctl_##a##_params(chain, dsp, __VA_ARGS__)
#define _DSP_IOCTL_STEP1(a, chain, dsp, ...)  \
					_DSP_IOCTL_STEP2(a, chain, dsp, __VA_ARGS__)
#define DSP_IOCTL(chain, dsp, ...)   \
			_DSP_IOCTL_STEP1(DSP_PP_NARG(__VA_ARGS__), chain, dsp, __VA_ARGS__)



uint8_t dsp_management_api_set_module_control(
			chain_handle_t chain_handle, char const*module_name,
			enum dsp_management_api_module_control_e ctl);


void dsp_management_api_delete_modules(void);



void dsp_management_api_set_chain_input_buffer(chain_handle_t chain_handle,
			uint16_t chain_input_pad_num, uint8_t *buffer, size_t buff_size );
void dsp_management_api_set_chain_output_buffer(chain_handle_t chain_handle,
			uint16_t chain_output_pad_num, uint8_t *buffer, size_t buff_size);

void dsp_management_api_init(uint8_t *zero_buff, size_t size_of_zero_buffer,
		uint8_t *dummy_buff, size_t size_of_dummy_buffer);
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
