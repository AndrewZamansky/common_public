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



#ifdef CONFIG_USE_HW_DSP
  #include "cpu_config.h"
  #include "arm_math.h"
#endif

static	void *dsp_buffers_pool;

/********  defines *********************/
#define NOT_ALLOCATED_BUFFER	255

/********  types  *********************/

/********  externals *********************/


/********  local defs *********************/



/**********   external variables    **************/



/***********   local variables    **************/

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
dsp_chain_t *DSP_CREATE_CHAIN(size_t max_num_of_dsp_modules)
{
	dsp_chain_t *pdsp_chain;
	pdsp_chain =  (dsp_chain_t*)malloc( sizeof(dsp_chain_t));
	pdsp_chain->dsp_chain =  (pdsp_descriptor*)malloc(max_num_of_dsp_modules * sizeof(pdsp_descriptor));
	pdsp_chain->max_num_of_dsp_modules = max_num_of_dsp_modules;
	pdsp_chain->occupied_dsp_modules = 0 ;
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
void DSP_ADD_MODULE_TO_CHAIN(dsp_chain_t *ap_chain, pdsp_descriptor dsp_module)
{
	size_t occupied_dsp_modules;

	occupied_dsp_modules = ap_chain->occupied_dsp_modules;
	while ( occupied_dsp_modules  == ap_chain->max_num_of_dsp_modules) ; // error trap

	ap_chain->dsp_chain[occupied_dsp_modules++] = dsp_module;

	ap_chain->occupied_dsp_modules = occupied_dsp_modules ;

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
	DSP_MANAGEMENT_API_module_control_t ctl;
	dsp_pad_t *curr_out_pad ;
	dsp_pad_t **in_pads ;
	dsp_pad_t *out_pads ;
	dsp_pad_t *curr_source_out_pad ;
	uint8_t i;

	ctl = dsp->ctl;
	in_pads = dsp->in_pads;
	out_pads = dsp->out_pads;

	for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		curr_out_pad = &out_pads[i];
		if(DSP_PAD_TYPE_NORMAL == curr_out_pad->pad_type)
		{
			curr_out_pad->buff = (float*)memory_pool_malloc(dsp_buffers_pool);
			curr_out_pad->sinks_processed_counter = curr_out_pad->total_registered_sinks;
		}
	}

	if(DSP_MANAGEMENT_API_MODULE_CONTROL_ON == ctl)
	{
		dsp->dsp_func(dsp->handle , len , in_pads , out_pads );
	}
	else if (DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
		{
			curr_out_pad = &out_pads[i];
			if(DSP_PAD_TYPE_NORMAL == curr_out_pad->pad_type)
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
			if(DSP_PAD_TYPE_NORMAL == curr_out_pad->pad_type)
			{
				my_float_memset(curr_out_pad->buff ,0,   len);
			}
		}
	}


	for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		curr_source_out_pad = in_pads[i];

		if(DSP_PAD_TYPE_NORMAL == curr_source_out_pad->pad_type)
		{
			while( 0 == curr_source_out_pad->sinks_processed_counter ) ; // debug trap
			curr_source_out_pad->sinks_processed_counter--;
			if(0 == curr_source_out_pad->sinks_processed_counter)
			{
				memory_pool_free(dsp_buffers_pool,curr_source_out_pad->buff );
			}
		}
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
	size_t i;
	pdsp_descriptor* dsp_chain;

	dsp_chain = ap_chain->dsp_chain;
	i = ap_chain->occupied_dsp_modules;
	while(i--)
	{
		DSP_PROCESS( *dsp_chain , len);
		dsp_chain++;
	}


}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_CREATE_LINK                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t DSP_CREATE_LINK(pdsp_descriptor source_dsp,DSP_OUTPUT_PADS_t source_dsp_pad,
		pdsp_descriptor sink_dsp,DSP_INPUT_PADS_t sink_dsp_pad)
{
	dsp_pad_t *p_curr_out_pad_of_source = &(source_dsp->out_pads[source_dsp_pad]) ;

	sink_dsp->in_pads[sink_dsp_pad] = p_curr_out_pad_of_source;

	p_curr_out_pad_of_source->pad_type = DSP_PAD_TYPE_NORMAL;
	p_curr_out_pad_of_source->total_registered_sinks++;
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_SET_SOURCE_BUFFER                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_SET_SOURCE_BUFFER(pdsp_descriptor source_dsp,DSP_INPUT_PADS_t source_dsp_pad, void *buffer)
{
	dsp_pad_t *p_curr_out_pad_of_source = &(source_dsp->out_pads[source_dsp_pad]) ;

	p_curr_out_pad_of_source->pad_type = DSP_PAD_TYPE_NOT_ALLOCATED_BUFFER;
	p_curr_out_pad_of_source->buff = (float*)buffer;

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
/* Function:        dsp_management_api_set_buffers_pool                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void dsp_management_api_set_buffers_pool(void *adsp_buffers_pool)
{

	dsp_buffers_pool = adsp_buffers_pool ;
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
void dsp_management_api_set_module_control(pdsp_descriptor dsp , DSP_MANAGEMENT_API_module_control_t ctl)
{
	dsp->ctl = ctl ;
}
