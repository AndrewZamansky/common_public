/*
 *
 * file :   downsampling_by_int_arm_cortex_m.c
 *
 *
 *
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "common_dsp_api.h"

#include "downsampling_by_int.h"
#include "downsampling_by_int_api.h"
#include "fir_filter_api.h"

extern "C" {
	#include "errors_api.h"
	#include "os_wrapper.h"
}

extern "C" {
	extern chain_handle_t downsampling_by_int_init_fir_chain(
			struct fir_filter_api_set_params_t *fir_set_params);
}

struct downsampling_by_int_t {
	uint32_t factor;
	chain_handle_t dsp_chain;
};


void downsampling_by_int_fir_function(void *p_filter,
	real_t *in_buf, real_t *tmp_buf, size_t in_buff_len, real_t *out_buf)
{
	chain_handle_t dsp_chain;
	uint32_t factor;
	size_t samples_in_buffer;
	size_t i;
	size_t j;

	dsp_chain = ((struct downsampling_by_int_t*)p_filter)->dsp_chain;

	dsp_management_api_set_chain_input_buffer(dsp_chain,
			IN_PAD(0), (uint8_t *)in_buf, in_buff_len);
	dsp_management_api_set_chain_output_buffer(dsp_chain,
			OUT_PAD(0), (uint8_t *)tmp_buf, in_buff_len);
	dsp_management_api_process_chain(dsp_chain);

	factor = ((struct downsampling_by_int_t*)p_filter)->factor;
	samples_in_buffer = in_buff_len / sizeof(real_t);

	j = 0;
	for (i = 0; i < samples_in_buffer; i += factor)
	{
		out_buf[j++] = tmp_buf[i];
	}
}



/*  func : downsampling_by_int_create()
 */
void downsampling_by_int_create_fir_filter(
		struct downsampling_by_int_instance_t *handle,
		struct fir_filter_api_set_params_t *fir_set_params)
{
	struct downsampling_by_int_t  *p_filter;
	chain_handle_t dsp_chain;

	p_filter = (struct downsampling_by_int_t *)os_safe_malloc(
								sizeof(struct downsampling_by_int_t));
	errors_api_check_if_malloc_succeed(p_filter);

	dsp_chain = downsampling_by_int_init_fir_chain(fir_set_params);
	p_filter->dsp_chain = dsp_chain;
	p_filter->factor = handle->factor;
	handle->p_downsampling_by_int_filter = p_filter;
}
