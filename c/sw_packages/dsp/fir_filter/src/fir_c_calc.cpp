/*
 *
 * file :   firs.c
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"
extern "C" {
	#include "errors_api.h"
	#include "os_wrapper.h"
}

#include "dsp_management_api.h"
#include "common_dsp_api.h"
#include "dsp_management_internal_api.h"
#include "fir_filter.h"
#include "fir_filter_api.h"

struct fir_filter_c_t {
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size;
	real_t *p_coefficients;
	real_t *state;
};


/*
 * y[n] = b[0] * x[n] + b[1] * x[n-1] +
 * 				 b[2] * x[n-2] + ...+ b[numTaps-1] * x[n-numTaps+1]
 */
void fir_filter_function(
		void *pFilter, real_t *apIn, real_t *apOut, size_t buff_len)
{
	struct fir_filter_c_t *fir_filter;
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size;
	real_t *p_coefficients;
	real_t *state;
	real_t out_val;
	real_t *state_new_input;
	size_t curr_len;
	real_t *start_of_samples;

	fir_filter = (struct fir_filter_c_t *)pFilter;
	number_of_filter_coefficients = fir_filter->number_of_filter_coefficients;
	predefined_data_block_size = fir_filter->predefined_data_block_size;
	p_coefficients = fir_filter->p_coefficients;
	state = fir_filter->state;
	state_new_input = &state[number_of_filter_coefficients - 1];

	while (buff_len)
	{
		curr_len = buff_len;
		if (predefined_data_block_size < curr_len)
		{
			curr_len = predefined_data_block_size;
		}

		for (size_t i = 0; i < curr_len; i++)
		{
			state_new_input[i] = apIn[i];
		}

		start_of_samples = state;
		for (size_t j = 0; j < curr_len; j++)
		{
			out_val = 0;
			for (size_t i = 0; i < number_of_filter_coefficients; i++)
			{
				out_val += start_of_samples[i] * p_coefficients[i];
			}
			*apOut++ = out_val;
			start_of_samples++;
		}

		// move samples to history
		for (size_t i = 0; i < number_of_filter_coefficients; i++)
		{
			state[i] = start_of_samples[i];
		}

		buff_len -= curr_len;
		apIn += curr_len;
	}
}


void *fir_alloc(size_t number_of_filter_coefficients,
		real_t *p_coefficients, size_t predefined_data_block_size)
{
	struct fir_filter_c_t *p_fir_filter;
	real_t *state;
	size_t state_len;

	p_fir_filter =
		(struct fir_filter_c_t *)os_safe_malloc(sizeof(struct fir_filter_c_t));
	errors_api_check_if_malloc_succeed(p_fir_filter);
	p_fir_filter->number_of_filter_coefficients = number_of_filter_coefficients;
	p_fir_filter->predefined_data_block_size = predefined_data_block_size;
	p_fir_filter->p_coefficients = p_coefficients;

	state_len = number_of_filter_coefficients + predefined_data_block_size - 1;
	state = (real_t *)os_safe_malloc(sizeof(real_t) * state_len);
	errors_api_check_if_malloc_succeed(p_fir_filter);
	for (size_t i = 0; i < state_len; i++) state[i] = (int16_t) 0;
	p_fir_filter->state = state;

	return p_fir_filter;
}


/*  func : fir_free()
 */
void fir_free(void *pFilter)
{
	if (NULL == pFilter) return;
	os_safe_free(((struct fir_filter_c_t *)pFilter)->state);
	os_safe_free(pFilter);
}
