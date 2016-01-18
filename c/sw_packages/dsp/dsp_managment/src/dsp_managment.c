/*
 *
 * file :   dsp_managment.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "dsp_managment_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "dsp_managment_api.h" // for device manager defines and typedefs
#include "_dsp_managment_prerequirements_check.h" // should be after {dsp_managment_config.h,dev_managment_api.h}

#include "dsp_managment_api.h" //place first to test that header file is self-contained
#include "dsp_managment.h"

#include "memory_pool_api.h"



#ifdef _USE_DSP_
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

#if 0
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_FUNC_1CH_IN_1CH_OUT                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_FUNC_1CH_IN_1CH_OUT(pdsp_descriptor dsp,void *ch1In,size_t	len)
{
	 DSP_MANAGMENT_API_module_control_t ctl;

	dsp->in_pads[0]	= (float*)ch1In;
	dsp->out_pads[0]	= (float*)memory_pool_malloc(dsp_buffers_pool);
	ctl = dsp->ctl;

	if(DSP_MANAGMENT_API_MODULE_CONTROL_ON == ctl)
	{
		dsp->dsp_func(dsp->handle,len,dsp->in_pads,dsp->out_pads );
	}
	else if (DSP_MANAGMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		my_float_memcpy(dsp->out_pads[0] , dsp->in_pads[0] , len);
	}
	else // if (DSP_MANAGMENT_API_MODULE_CONTROL_MUTE == ctl)
	{
		my_float_memset(dsp->out_pads[0] ,0 , len);
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_FUNC_2CH_IN_2CH_OUT                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_FUNC_2CH_IN_2CH_OUT(pdsp_descriptor dsp,void *ch1In,void *ch2In,size_t	len)
{
	 DSP_MANAGMENT_API_module_control_t ctl;

	dsp->in_pads[0]	= (float*)ch1In;
	dsp->in_pads[1]	= (float*)ch2In;
	dsp->out_pads[0]	= (float*)memory_pool_malloc(dsp_buffers_pool);
	dsp->out_pads[1]	= (float*)memory_pool_malloc(dsp_buffers_pool);
	ctl = dsp->ctl;
	if(DSP_MANAGMENT_API_MODULE_CONTROL_ON == ctl)
	{
		dsp->dsp_func(dsp->handle,len,dsp->in_pads,dsp->out_pads );
	}
	else if (DSP_MANAGMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		my_float_memcpy_2_buffers(dsp->out_pads[0] , dsp->in_pads[0] ,
				dsp->out_pads[1] , dsp->in_pads[1] , len);
	}
	else // if (DSP_MANAGMENT_API_MODULE_CONTROL_MUTE == ctl)
	{
		my_float_memset(dsp->out_pads[0] ,0 , len);
		my_float_memset(dsp->out_pads[1] ,0 , len);
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_FUNC_2CH_IN_1CH_OUT                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_FUNC_2CH_IN_1CH_OUT(pdsp_descriptor dsp,void *ch1In,void *ch2In,size_t	len)
{
	 DSP_MANAGMENT_API_module_control_t ctl;

	dsp->in_pads[0]	= (float*)ch1In;
	dsp->in_pads[1]	= (float*)ch2In;
	dsp->out_pads[0]	= (float*)memory_pool_malloc(dsp_buffers_pool);
	ctl = dsp->ctl;
	if(DSP_MANAGMENT_API_MODULE_CONTROL_ON == ctl)
	{
		dsp->dsp_func(dsp->handle,len,dsp->in_pads,dsp->out_pads );
	}
	else if (DSP_MANAGMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		my_float_memcpy(dsp->out_pads[0] , dsp->in_pads[0],   len);
	}
	else // if (DSP_MANAGMENT_API_MODULE_CONTROL_MUTE == ctl)
	{
		my_float_memset(dsp->out_pads[0] ,0 , len);
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_FUNC_2CH_IN_1CH_OUT_NO_OUTPUT_ALLOCATION                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_FUNC_2CH_IN_1CH_OUT_NO_OUTPUT_ALLOCATION(pdsp_descriptor dsp,void *ch1In,void *ch2In,void *ch1Out,size_t	len)
{
	 DSP_MANAGMENT_API_module_control_t ctl;

	dsp->in_pads[0]	= (float*)ch1In;
	dsp->in_pads[1]	= (float*)ch2In;
	dsp->out_pads[0]	= (float*)ch1Out;
	ctl = dsp->ctl;
	if(DSP_MANAGMENT_API_MODULE_CONTROL_ON == ctl)
	{

		dsp->dsp_func(dsp->handle,len,dsp->in_pads,dsp->out_pads );
	}
	else if (DSP_MANAGMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		my_float_memcpy(dsp->out_pads[0] , dsp->in_pads[0],   len);
	}
	else // if (DSP_MANAGMENT_API_MODULE_CONTROL_MUTE == ctl)
	{
		my_float_memset(dsp->out_pads[0] ,0 , len);
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        DSP_FUNC_1CH_IN_2CH_OUT                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void DSP_FUNC_1CH_IN_2CH_OUT(pdsp_descriptor dsp,void *ch1In,size_t	len)
{
	 DSP_MANAGMENT_API_module_control_t ctl;

	dsp->in_pads[0]	= (float*)ch1In;
	dsp->out_pads[0]	= (float*)memory_pool_malloc(dsp_buffers_pool);
	dsp->out_pads[1]	= (float*)memory_pool_malloc(dsp_buffers_pool);
	ctl = dsp->ctl;
	if(DSP_MANAGMENT_API_MODULE_CONTROL_ON == ctl)
	{

		dsp->dsp_func(dsp->handle,len,dsp->in_pads,dsp->out_pads );
	}
	else if (DSP_MANAGMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		my_float_memcpy(dsp->out_pads[0] , dsp->in_pads[0],   len);
		my_float_memcpy(dsp->out_pads[1] , dsp->in_pads[0],   len);
	}
	else // if (DSP_MANAGMENT_API_MODULE_CONTROL_MUTE == ctl)
	{
		my_float_memset(dsp->out_pads[0] ,0 , len);
		my_float_memset(dsp->out_pads[1] ,0 , len);
	}
}

#endif
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
	DSP_MANAGMENT_API_module_control_t ctl;
	dsp_pad_t *curr_out_pad ;
	dsp_pad_t *curr_source_out_pad ;
	uint8_t i;

	for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		curr_out_pad = &dsp->out_pads[i];
		if(DSP_PAD_TYPE_NORMAL == curr_out_pad->pad_type)
		{
			curr_out_pad->buff = (float*)memory_pool_malloc(dsp_buffers_pool);
			curr_out_pad->sinks_processed_counter = curr_out_pad->total_registered_sinks;
		}
	}

	ctl = dsp->ctl;
	if(DSP_MANAGMENT_API_MODULE_CONTROL_ON == ctl)
	{
		dsp->dsp_func(dsp->handle,len,dsp->in_pads,dsp->out_pads );
	}
	else if (DSP_MANAGMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
		{
			curr_out_pad = &dsp->out_pads[i];
			if(DSP_PAD_TYPE_NORMAL == curr_out_pad->pad_type)
			{
				my_float_memcpy(curr_out_pad->buff , dsp->in_pads[0]->buff,   len);
			}
		}
	}
	else // if (DSP_MANAGMENT_API_MODULE_CONTROL_MUTE == ctl)
	{
		for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
		{
			curr_out_pad = &dsp->out_pads[i];
			if(DSP_PAD_TYPE_NORMAL == curr_out_pad->pad_type)
			{
				my_float_memset(curr_out_pad->buff ,0,   len);
			}
		}
	}


	for(i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		curr_source_out_pad = dsp->in_pads[i];

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
	sink_dsp->in_pads[sink_dsp_pad] = &(source_dsp->out_pads[source_dsp_pad]);

	source_dsp->out_pads[source_dsp_pad].pad_type = DSP_PAD_TYPE_NORMAL;
	source_dsp->out_pads[source_dsp_pad].total_registered_sinks++;
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
	source_dsp->out_pads[source_dsp_pad].pad_type = DSP_PAD_TYPE_NOT_ALLOCATED_BUFFER;
	source_dsp->out_pads[source_dsp_pad].buff = (float*)buffer;

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
void DSP_SET_SINK_BUFFER(pdsp_descriptor dsp,DSP_INPUT_PADS_t dsp_output_pad, void *buffer)
{
	dsp->out_pads[dsp_output_pad].pad_type = DSP_PAD_TYPE_NOT_ALLOCATED_BUFFER;
	dsp->out_pads[dsp_output_pad].buff = (float*)buffer;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        dsp_managment_api_set_buffers_pool                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void dsp_managment_api_set_buffers_pool(void *adsp_buffers_pool)
{

	dsp_buffers_pool = adsp_buffers_pool ;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        dsp_managment_api_set_module_control                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void dsp_managment_api_set_module_control(pdsp_descriptor dsp , DSP_MANAGMENT_API_module_control_t ctl)
{
	dsp->ctl = ctl ;
}
