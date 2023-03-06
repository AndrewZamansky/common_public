/*
 * file : DOWNSAMPLING_BY_INT_FILTER.h
 *
 *
 *
 */

#ifndef _DOWNSAMPLING_BY_INT_FILTER_H
#define _DOWNSAMPLING_BY_INT_FILTER_H

#include "dsp_management_api.h"
#include "common_dsp_api.h"
#include "dsp_management_internal_api.h"
#include "downsampling_by_int_api.h"


struct downsampling_by_int_instance_t {
	void *p_downsampling_by_int_filter;
	uint32_t factor;
	enum downsampling_filter_type_e filter_type;
};


void downsampling_by_int_create_fir_filter(
		struct downsampling_by_int_instance_t *handle,
		struct fir_filter_api_set_params_t *fir_set_params);

void downsampling_by_int_create_biquad_filter(
		struct downsampling_by_int_instance_t *handle, size_t num_of_bands,
		struct biquad_filter_api_band_raw_coeff_float_t *raw_coeffs);

void downsampling_by_int_fir_function(void *p_filter,
	real_t *in_buf, real_t *tmp_buf, size_t in_buff_len, real_t *out_buf);

void downsampling_by_int_biquad_function(void *p_filter,
	real_t *in_buf, real_t *tmp_buf, size_t in_buff_len, real_t *out_buf);



#endif
