
#ifndef _dsp_management_internal_api_h_
#define _dsp_management_internal_api_h_

#include "_project_typedefs.h"
#include "_project_defines.h"



/**********  define API  types ************/



void _dsp_register_new_module_type(const char *a_module_name,
		dsp_ioctl_func_t a_ioctle_func, dsp_func_t a_dsp_func,
		uint16_t a_module_data_size);

#define DSP_REGISTER_NEW_MODULE(name, ioctl_f, dsp_f, module_data_t)    \
	_dsp_register_new_module_type(name, ioctl_f, dsp_f, sizeof(module_data_t))




void dsp_get_buffer_from_pad(
		struct dsp_pad_t *pad, real_t **buff, size_t *buff_len);

#endif
