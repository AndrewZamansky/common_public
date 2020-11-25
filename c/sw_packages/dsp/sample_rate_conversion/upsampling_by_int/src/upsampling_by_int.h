/*
 * file : UPSAMPLING_BY_INT_FILTER.h
 *
 *
 *
 */

#ifndef _UPSAMPLING_BY_INT_FILTER_H
#define _UPSAMPLING_BY_INT_FILTER_H

#include "dsp_management_api.h"
#include "common_dsp_api.h"
#include "dsp_management_internal_api.h"

struct upsampling_by_int_instance_t {
	void *p_upsampling_by_int_filter;
	uint32_t factor;
};


void upsampling_by_int_create(
		struct upsampling_by_int_instance_t *handle,
		struct fir_filter_api_set_params_t *fir_set_params);

void upsampling_by_int_function(void *p_filter,
	real_t *in_buf, real_t *tmp_buf, size_t in_buff_len, real_t *out_buf);


#endif
