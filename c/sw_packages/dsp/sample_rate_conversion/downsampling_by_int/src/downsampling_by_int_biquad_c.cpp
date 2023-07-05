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
#include "biquad_filter_api.h"

extern "C" {
	#include "errors_api.h"
	#include "os_wrapper.h"
}

extern "C" {
	extern chain_handle_t downsampling_by_int_init_biquad_chain(
			size_t num_of_bands);
	extern char biquad_filter[];
}

struct downsampling_by_int_t {
	uint32_t factor;
	chain_handle_t dsp_chain;
};


void downsampling_by_int_biquad_function(void *p_filter,
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
void downsampling_by_int_create_biquad_filter(
		struct downsampling_by_int_instance_t *handle, size_t num_of_bands,
		struct biquad_filter_api_band_set_raw_t *raw_coeffs)
{
	chain_handle_t dsp_chain;
	struct downsampling_by_int_t  *p_filter;

	p_filter = (struct downsampling_by_int_t *)os_safe_malloc(
								sizeof(struct downsampling_by_int_t));
	errors_api_check_if_malloc_succeed(p_filter);

	dsp_chain = downsampling_by_int_init_biquad_chain(num_of_bands);
	p_filter->dsp_chain = dsp_chain;
	p_filter->factor = handle->factor;
	handle->p_downsampling_by_int_filter = p_filter;

	for (size_t i = 0; i < num_of_bands; i++)
	{
		DSP_IOCTL(dsp_chain, biquad_filter,
				IOCTL_BIQUAD_FILTER_SET_BAND_RAW_COEFFICIENTS, &raw_coeffs[i]);
	}
}
