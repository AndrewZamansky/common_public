/*
 *
 * file :   dsp_management.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_dsp_management_prerequirements_check.h"

#include "dsp_management.h"

#include "memory_pool_api.h"
#include "string.h"



static	void *dsp_buffers_pool = NULL;

/********  defines *********************/
#define NOT_ALLOCATED_BUFFER	255

/********  types  *********************/

/********  externals *********************/



/**********   external variables    **************/



/***********   local variables    **************/

uint8_t size_of_module_array = 0;
dsp_module_t *dsp_module_array;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        my_float_memcpy                                                                          */
// on gcc cortex-m3 MEMCPY() is slower then direct copy !!!
/*---------------------------------------------------------------------------------------------------------*/
static void my_float_memcpy(float *dest ,float *src , size_t len)
{
	for( ; len ;len--)
	{
		*dest++ = *src++;
	}
}

#if 0
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        my_float_memcpy                                                                          */
// on gcc cortex-m3 MEMCPY() is slower then direct copy !!!
/*---------------------------------------------------------------------------------------------------------*/
static void my_float_memcpy_2_buffers(float *dest1 ,float *src1 ,float *dest2 ,float *src2, size_t len)
{
	for( ; len ;len--)
	{
		*dest1++ = *src1++;
		*dest2++ = *src2++;
	}
}
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        my_float_memset                                                                          */
// on gcc cortex-m3 MEMCPY() is slower then direct copy !!!
/*---------------------------------------------------------------------------------------------------------*/
static void my_float_memset(float *dest ,float val , size_t len)
{
	for( ; len ;len--)
	{
		*dest++ = val;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_REGISTER_NEW_MODULE                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void _DSP_REGISTER_NEW_MODULE(char *a_module_name, dsp_ioctl_func_t a_ioctle_func , dsp_func_t a_dsp_func
		, uint16_t a_module_data_size)
{
	dsp_module_t* p_new_dsp_module;
	size_of_module_array++;
	dsp_module_array = (dsp_module_t*)realloc(dsp_module_array, sizeof(dsp_module_t) * size_of_module_array);
	p_new_dsp_module = &dsp_module_array[size_of_module_array-1];
	p_new_dsp_module->name = a_module_name;
	p_new_dsp_module->ioctl = a_ioctle_func;
	p_new_dsp_module->dsp_func = a_dsp_func;
	p_new_dsp_module->module_data_size = a_module_data_size;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_CREATE_CHAIN                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
dsp_chain_t *DSP_CREATE_CHAIN(size_t max_num_of_dsp_modules , void *adsp_buffers_pool)
{
	dsp_chain_t *pdsp_chain;
	uint8_t i;

	if(NULL == dsp_buffers_pool)
	{
		dsp_buffers_pool = adsp_buffers_pool ;
	}
	pdsp_chain =  (dsp_chain_t*)malloc( sizeof(dsp_chain_t));
	pdsp_chain->dsp_chain =  (pdsp_descriptor*)malloc(max_num_of_dsp_modules * sizeof(pdsp_descriptor));
	pdsp_chain->max_num_of_dsp_modules = max_num_of_dsp_modules;
	pdsp_chain->occupied_dsp_modules = 0 ;
	pdsp_chain->chain_in_pads[MAX_NUM_OF_OUTPUT_PADS].pad_type = DSP_PAD_TYPE_NOT_ALLOCATED_BUFFER;
	for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		pdsp_chain->chain_out_pads[i] = NULL;
		pdsp_chain->out_buffers[i] = NULL;
	}
	return pdsp_chain;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_ADD_MODULE_TO_CHAIN                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_ADD_MODULE_TO_CHAIN(dsp_chain_t *ap_chain, char *a_module_name,  pdsp_descriptor dsp_module)
{
	uint8_t i;
	size_t occupied_dsp_modules;
	dsp_module_t* p_dsp_module;
	uint8_t retVal;
	dsp_pad_t **in_pads;
	dsp_pad_t *out_pads;
	dsp_pad_t *curr_out_pad;

	occupied_dsp_modules = ap_chain->occupied_dsp_modules;
	while ( occupied_dsp_modules  == ap_chain->max_num_of_dsp_modules) ; // error trap

	for(i=0 ; i < size_of_module_array ; i++)
	{
		p_dsp_module = &dsp_module_array[i];
		if(0==strcmp(a_module_name,p_dsp_module->name))
		{
			in_pads = dsp_module->in_pads;
			out_pads = dsp_module->out_pads;
			for (i = 0; i<MAX_NUM_OF_OUTPUT_PADS; i++)
			{
				in_pads[i] = &ap_chain->chain_in_pads[MAX_NUM_OF_OUTPUT_PADS];// set to default zero's input buffer
				curr_out_pad = &out_pads[i];
				curr_out_pad->pad_type = DSP_PAD_TYPE_NOT_USED;
				curr_out_pad->total_registered_sinks = 0;
				curr_out_pad->sinks_processed_counter = 0;
				curr_out_pad->buff = NULL;
			}

			dsp_module->ctl = DSP_MANAGEMENT_API_MODULE_CONTROL_ON;
			dsp_module->ioctl = p_dsp_module->ioctl;
			dsp_module->dsp_func = p_dsp_module->dsp_func;
			dsp_module->handle =malloc( p_dsp_module->module_data_size );
			retVal = DSP_IOCTL_0_PARAMS(dsp_module , IOCTL_DSP_INIT );
			if (retVal) while(1);// error trap in case dsp module failed to start

			ap_chain->dsp_chain[occupied_dsp_modules++] = dsp_module;

			ap_chain->occupied_dsp_modules = occupied_dsp_modules ;
			return ;
		}
	}
	while(1);// error trap in case module with a_module_name string was not found
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_PROCESS                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_PROCESS(pdsp_descriptor dsp , size_t	len)
{
	dsp_management_api_module_control_t ctl;
	dsp_pad_t *curr_out_pad ;
	dsp_pad_t **in_pads ;
	dsp_pad_t *out_pads ;
	dsp_pad_t *curr_source_out_pad ;
	uint8_t i;
	float *dummy_output_buff = NULL;
	uint8_t dummy_output_buff_allocated = 0;

	ctl = dsp->ctl;
	in_pads = dsp->in_pads;
	out_pads = dsp->out_pads;

	for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		uint8_t pad_type;

		curr_out_pad = &out_pads[i];
		pad_type = curr_out_pad->pad_type;
		if(DSP_PAD_TYPE_NORMAL == pad_type)
		{
			curr_out_pad->buff = (float*)memory_pool_malloc(dsp_buffers_pool);
			curr_out_pad->sinks_processed_counter = curr_out_pad->total_registered_sinks;
		}
		else if(DSP_PAD_TYPE_NOT_USED == pad_type)
		{
			if( 0 == dummy_output_buff_allocated)
			{
				dummy_output_buff = (float*)memory_pool_malloc(dsp_buffers_pool);
				dummy_output_buff_allocated = 1;
			}
			curr_out_pad->buff = dummy_output_buff;
		}
	}

	if(DSP_MANAGEMENT_API_MODULE_CONTROL_ON == ctl)
	{
		dsp->dsp_func(dsp , len , in_pads , out_pads );
	}
	else if (DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
		{
			curr_out_pad = &out_pads[i];
			if (DSP_PAD_TYPE_NOT_USED != curr_out_pad->pad_type)
			{
				my_float_memcpy(curr_out_pad->buff , in_pads[0]->buff,   len);
			}
		}
	}
	else // if (DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE == ctl)
	{
		for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
		{
			curr_out_pad = &out_pads[i];
			if (DSP_PAD_TYPE_NOT_USED != curr_out_pad->pad_type)
			{
				my_float_memset(curr_out_pad->buff ,0,   len);
			}
		}
	}


	for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		curr_source_out_pad = in_pads[i];

		if  (DSP_PAD_TYPE_NORMAL == curr_source_out_pad->pad_type)
		{
			while( 0 == curr_source_out_pad->sinks_processed_counter ) ; // debug trap
			curr_source_out_pad->sinks_processed_counter--;
			if(0 == curr_source_out_pad->sinks_processed_counter)
			{
				memory_pool_free(dsp_buffers_pool , curr_source_out_pad->buff );
			}
		}
	}

	if( 1 == dummy_output_buff_allocated)
	{
		memory_pool_free(dsp_buffers_pool , dummy_output_buff);
	}

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_PROCESS_CHAIN                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_PROCESS_CHAIN(dsp_chain_t *ap_chain , size_t	len )
{
	dsp_pad_t *curr_source_out_pad ;
	size_t i;
	pdsp_descriptor* dsp_chain;
	float *zeros_buff;
	dsp_chain = ap_chain->dsp_chain;
	i = ap_chain->occupied_dsp_modules;

	zeros_buff = (float*)memory_pool_malloc(dsp_buffers_pool);
	my_float_memset(zeros_buff , 0 , len);
	ap_chain->chain_in_pads[MAX_NUM_OF_OUTPUT_PADS].buff = zeros_buff;


	while(i--)
	{
		DSP_PROCESS( *dsp_chain , len);
		dsp_chain++;
	}

	for(i = 0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		float *out_buff;
		curr_source_out_pad = ap_chain->chain_out_pads[i];

		if (NULL == curr_source_out_pad)
		{
			continue;
		}

		out_buff = ap_chain->out_buffers[i];
		if (NULL != out_buff)
		{
			my_float_memcpy(out_buff , curr_source_out_pad->buff, len);
		}

		if  (DSP_PAD_TYPE_NORMAL == curr_source_out_pad->pad_type)
		{
			while( 0 == curr_source_out_pad->sinks_processed_counter ) ; // debug trap
			curr_source_out_pad->sinks_processed_counter--;
			if(0 == curr_source_out_pad->sinks_processed_counter)
			{
				memory_pool_free(dsp_buffers_pool , curr_source_out_pad->buff );
			}
		}
	}

	memory_pool_free(dsp_buffers_pool,zeros_buff );

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_CREATE_INTER_MODULES_LINK                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DSP_CREATE_INTER_MODULES_LINK(pdsp_descriptor source_dsp,DSP_OUTPUT_PADS_t source_dsp_pad,
		pdsp_descriptor sink_dsp,DSP_INPUT_PADS_t sink_dsp_pad)
{
	dsp_pad_t *p_curr_out_pad_of_source = &(source_dsp->out_pads[source_dsp_pad]) ;

	sink_dsp->in_pads[sink_dsp_pad] = p_curr_out_pad_of_source;

	p_curr_out_pad_of_source->pad_type = DSP_PAD_TYPE_NORMAL;
	p_curr_out_pad_of_source->total_registered_sinks++;
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_CREATE_CHAIN_INPUT_TO_MODULE_LINK                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DSP_CREATE_CHAIN_INPUT_TO_MODULE_LINK(dsp_chain_t *ap_chain,DSP_INPUT_PADS_t source_dsp_pad,
		pdsp_descriptor sink_dsp,DSP_INPUT_PADS_t sink_dsp_pad)
{
	dsp_pad_t *p_curr_out_pad_of_source = &(ap_chain->chain_in_pads[source_dsp_pad]) ;

	sink_dsp->in_pads[sink_dsp_pad] = p_curr_out_pad_of_source;

	p_curr_out_pad_of_source->pad_type = DSP_PAD_TYPE_NOT_ALLOCATED_BUFFER;
	p_curr_out_pad_of_source->total_registered_sinks++;
	return 0;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_CREATE_MODULE_TO_CHAIN_OUTPUT_LINK                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DSP_CREATE_MODULE_TO_CHAIN_OUTPUT_LINK(dsp_chain_t *ap_chain,DSP_OUTPUT_PADS_t sink_dsp_pad,
		pdsp_descriptor source_dsp,DSP_OUTPUT_PADS_t source_dsp_pad)
{
	dsp_pad_t *p_curr_out_pad_of_source = &(source_dsp->out_pads[source_dsp_pad]) ;

	ap_chain->chain_out_pads[sink_dsp_pad] = p_curr_out_pad_of_source;

	p_curr_out_pad_of_source->pad_type = DSP_PAD_TYPE_NORMAL;
	p_curr_out_pad_of_source->total_registered_sinks++;
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_SET_CHAIN_INPUT_BUFFER                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_SET_CHAIN_INPUT_BUFFER(dsp_chain_t *ap_chain,DSP_INPUT_PADS_t sink_dsp_pad, void *buffer)
{
	dsp_pad_t *p_curr_out_pad_of_source = &(ap_chain->chain_in_pads[sink_dsp_pad]) ;

	p_curr_out_pad_of_source->buff = (float*)buffer;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_SET_CHAIN_INPUT_BUFFER                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_SET_CHAIN_OUTPUT_BUFFER(dsp_chain_t *ap_chain,DSP_OUTPUT_PADS_t output_dsp_pad, void *buffer)
{
	ap_chain->out_buffers[output_dsp_pad] = (float*)buffer;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_SET_SINK_BUFFER                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_SET_SINK_BUFFER(pdsp_descriptor dsp,DSP_OUTPUT_PADS_t dsp_output_pad, void *buffer)
{
	dsp_pad_t *p_curr_out_pad = &(dsp->out_pads[dsp_output_pad]) ;

	p_curr_out_pad->pad_type = DSP_PAD_TYPE_NOT_ALLOCATED_BUFFER;
	p_curr_out_pad->buff = (float*)buffer;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        dsp_management_api_set_module_control                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void dsp_management_api_set_module_control(pdsp_descriptor dsp , dsp_management_api_module_control_t ctl)
{
	dsp->ctl = ctl ;
}
