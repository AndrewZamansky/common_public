/*
 * file : DOWNSAMPLING_BY_INT_FILTER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _DOWNSAMPLING_BY_INT_FILTER_H
#define _DOWNSAMPLING_BY_INT_FILTER_H

#include "src/_downsampling_by_int_prerequirements_check.h"
#include "downsampling_by_int_api.h"


/***************   typedefs    *******************/


typedef struct {
	void *p_downsampling_by_int_filter   ;
	void *p_coefficients   ;
	size_t factor ;
	size_t number_of_filter_coefficients ;
	size_t predefined_data_block_size ;
} DOWNSAMPLING_BY_INT_Instance_t;


void *downsampling_by_int_alloc(size_t factor , size_t number_of_filter_coefficients,
		float *  	p_coefficients ,size_t predefined_data_block_size);

void downsampling_by_int_free(void *pFilter);

void downsampling_by_int_function(void *pFilter,float *pin,float *pOut,size_t buff_len);




#endif /* */
