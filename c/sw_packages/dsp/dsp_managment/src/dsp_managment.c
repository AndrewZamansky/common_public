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
