/*
 * file : FIR_FILTER.h
 *
 *
 */

#ifndef _FIR_FILTER_H
#define _FIR_FILTER_H


struct fir_filter_t {
	void *p_fir_filter;
	real_t *p_coefficients;
	size_t number_of_filter_coefficients;
	size_t predefined_data_block_size;
};


void *fir_alloc(size_t number_of_filter_coefficients,
		float * p_coefficients, size_t predefined_data_block_size);

void fir_free(void *pFilter );

void fir_filter_function(void *pFilter,
							float *pin, float *pOut, size_t buff_len);




#endif
