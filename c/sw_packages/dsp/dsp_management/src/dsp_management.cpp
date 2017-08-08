/*
 *
 * file :   dsp_management.c
 *
 *
 */

/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dsp_management_api.h"
#include "dsp_management_internal_api.h"

#include "dsp_management.h"

#include "string.h"

#include "auto_init_api.h"

extern "C" {
#include "memory_pool_api.h"
#include "os_wrapper.h"
}

#include "_dsp_management_prerequirements_check.h"

static	void *dsp_buffers_pool = NULL;

/********  defines *********************/
#define NOT_ALLOCATED_BUFFER	255

/********  types  *********************/

/********  externals *********************/



/**********   external variables    **************/



/***********   local variables    **************/

uint8_t size_of_module_array = 0;
struct dsp_module_t *dsp_module_array = NULL;
struct dsp_pad_t default_zero_buff = {NULL, DSP_OUT_PAD_TYPE_NOT_USED, 0, 0};

/**
 * my_memcpy()
 *
 * on gcc cortex-m3 MEMCPY() is slower then direct copy !!!
 * return:
 */
static void my_memcpy(real_t *dest ,real_t *src , size_t len)
{
	while (len--)
	{
		*dest++ = *src++;
	}
}

#if 0
/**
 * my_memcpy_2_buffers()
 *
 * on gcc cortex-m3 MEMCPY() is slower then direct copy !!!
 * return:
 */
static void my_memcpy_2_buffers(real_t *dest1, real_t *src1,
								real_t *dest2, real_t *src2, size_t len)
{
	while (len--)
	{
		*dest1++ = *src1++;
		*dest2++ = *src2++;
	}
}
#endif


/**
 * my_memset()
 *
 * on gcc cortex-m3 MEMSET() is slower then direct copy !!!
 * return:
 */
static void my_memset(real_t *dest ,real_t val , size_t len)
{
	while (len--)
	{
		*dest++ = val;
	}
}


/**
 * _DSP_REGISTER_NEW_MODULE()
 *
 * return:
 */
void _DSP_REGISTER_NEW_MODULE(const char *a_module_name,
		dsp_ioctl_func_t a_ioctle_func, dsp_func_t a_dsp_func,
		uint16_t a_module_data_size)
{
	struct dsp_module_t* p_new_dsp_module;
	size_of_module_array++;
	dsp_module_array = (struct dsp_module_t*)os_safe_realloc(dsp_module_array,
			sizeof(struct dsp_module_t) * size_of_module_array);
	p_new_dsp_module = &dsp_module_array[size_of_module_array - 1];
	p_new_dsp_module->name = a_module_name;
	p_new_dsp_module->ioctl = a_ioctle_func;
	p_new_dsp_module->dsp_func = a_dsp_func;
	p_new_dsp_module->module_data_size = a_module_data_size;
}


/**
 * DSP_DELETE_MODULES()
 *
 * return:
 */
void DSP_DELETE_MODULES()
{
	os_safe_free(dsp_module_array);
	dsp_module_array = NULL;
}


/**
 * DSP_CREATE_CHAIN()
 *
 * return:
 */
struct dsp_chain_t *DSP_CREATE_CHAIN(size_t max_num_of_dsp_modules,
		size_t max_num_of_allocated_buffers)
{
	struct dsp_chain_t *pdsp_chain;
	uint8_t i;

	if (NULL == dsp_buffers_pool)
	{
		dsp_buffers_pool = memory_pool_init(max_num_of_allocated_buffers,
				I2S_BUFF_LEN * sizeof(real_t));
	}
	if (NULL == default_zero_buff.buff)
	{
		default_zero_buff.buff = (real_t*)memory_pool_zmalloc(dsp_buffers_pool);
		default_zero_buff.buff_size =
				memory_pool_get_chunk_size(dsp_buffers_pool) / sizeof(real_t);
		default_zero_buff.pad_type = DSP_PAD_TYPE_DUMMY_ZERO_BUFFER;
	}

	pdsp_chain =  (struct dsp_chain_t*)os_safe_malloc( sizeof(struct dsp_chain_t));
	pdsp_chain->dsp_chain = (struct dsp_desc_t**)os_safe_malloc(
			max_num_of_dsp_modules * sizeof(struct dsp_desc_t));
	pdsp_chain->max_num_of_dsp_modules = max_num_of_dsp_modules;
	pdsp_chain->occupied_dsp_modules = 0 ;

	for (i=0; i<MAX_NUM_OF_OUTPUT_PADS; i++)
	{
		pdsp_chain->chain_in_pads[i].pad_type =
				DSP_PAD_TYPE_CHAIN_INPUT_BUFFER;
		pdsp_chain->chain_in_pads[i].buff =	NULL;
		pdsp_chain->chain_in_pads[i].total_registered_sinks =	0;
		pdsp_chain->chain_in_pads[i].sinks_processed_counter =	0;
		pdsp_chain->chain_out_pads[i] = NULL;
//		pdsp_chain->chain_out_pads[i] = &default_zero_buff;
//		pdsp_chain->out_buffers[i] = NULL;
	}
	return pdsp_chain;
}


/**
 * DSP_DELETE_CHAIN()
 *
 * return:
 */
void DSP_DELETE_CHAIN(struct dsp_chain_t * ap_chain)
{
	size_t i;
	size_t occupied_dsp_modules;
	struct dsp_desc_t **dsp_module;

	occupied_dsp_modules = ap_chain->occupied_dsp_modules;
	dsp_module = ap_chain->dsp_chain;
	for (i=0 ; i < occupied_dsp_modules ; i++)
	{
		DSP_IOCTL_0_PARAMS(*dsp_module , IOCTL_DSP_DELETE );
		os_safe_free((*dsp_module)->handle);
		dsp_module++;
	}
	os_safe_free(ap_chain->dsp_chain);
	os_safe_free(ap_chain);
}


/**
 * DSP_ADD_MODULE_TO_CHAIN()
 *
 * return:
 */
void DSP_ADD_MODULE_TO_CHAIN(struct dsp_chain_t *ap_chain,
		char *a_module_name,  struct dsp_desc_t *dsp_module)
{
	uint8_t i;
	size_t occupied_dsp_modules;
	struct dsp_module_t* p_dsp_module;
	uint8_t retVal;
	struct dsp_pad_t **in_pads;
	struct dsp_pad_t *out_pads;
	struct dsp_pad_t *curr_out_pad;

	occupied_dsp_modules = ap_chain->occupied_dsp_modules;
	if ( occupied_dsp_modules  == ap_chain->max_num_of_dsp_modules)
	{
		CRITICAL_ERROR("too many modules");
	}

	for (i=0; i < size_of_module_array; i++)
	{
		p_dsp_module = &dsp_module_array[i];
		if (0 != strcmp(a_module_name,p_dsp_module->name))
		{
			continue;
		}

		in_pads = dsp_module->in_pads;
		out_pads = dsp_module->out_pads;
		for (i = 0; i < MAX_NUM_OF_OUTPUT_PADS; i++)
		{
			// set to default zero's input buffer
			in_pads[i] = &default_zero_buff;

			curr_out_pad = &out_pads[i];
			curr_out_pad->pad_type = DSP_OUT_PAD_TYPE_NOT_USED;
			curr_out_pad->total_registered_sinks = 0;
			curr_out_pad->sinks_processed_counter = 0;
			curr_out_pad->buff = NULL;
			curr_out_pad->buff_size = 0;
		}

		dsp_module->ctl = DSP_MANAGEMENT_API_MODULE_CONTROL_ON;
		dsp_module->ioctl = p_dsp_module->ioctl;
		dsp_module->dsp_func = p_dsp_module->dsp_func;
		dsp_module->handle = os_safe_malloc( p_dsp_module->module_data_size );
		retVal = DSP_IOCTL_0_PARAMS(dsp_module , IOCTL_DSP_INIT );

		// error trap in case dsp module failed to start
		if (retVal)
		{
			CRITICAL_ERROR("dsp module failed to start");
		}


		ap_chain->dsp_chain[occupied_dsp_modules++] = dsp_module;

		ap_chain->occupied_dsp_modules = occupied_dsp_modules ;
		return ;
	}

	// error trap in case module with a_module_name string was not found
	CRITICAL_ERROR("module with a_module_name string was not found");
}



void release_unused_buffers(struct dsp_pad_t **in_pads)
{
	int i;
	struct dsp_pad_t *curr_source_out_pad ;

	for (i=0; i<MAX_NUM_OF_OUTPUT_PADS ;i++)
	{
		curr_source_out_pad = in_pads[i];

		if (DSP_OUT_PAD_TYPE_NORMAL == curr_source_out_pad->pad_type)
		{
			if ( 0 == curr_source_out_pad->sinks_processed_counter )
			{
				CRITICAL_ERROR("counter already reached zero");
			}

			curr_source_out_pad->sinks_processed_counter--;
			if (0 == curr_source_out_pad->sinks_processed_counter)
			{
				memory_pool_free(dsp_buffers_pool, curr_source_out_pad->buff);
				curr_source_out_pad->buff = NULL;
				curr_source_out_pad->buff_size = 0;
			}
		}
	}
}


/**
 * DSP_PROCESS()
 *
 * return:
 */
void DSP_PROCESS(struct dsp_desc_t *dsp )
{
	dsp_management_api_module_control_t ctl;
	struct dsp_pad_t *curr_out_pad ;
	struct dsp_pad_t **in_pads ;
	struct dsp_pad_t *out_pads ;
	uint8_t i;
	real_t *dummy_output_buff = NULL;
	uint8_t dummy_output_buff_allocated = 0;
	size_t buff_size;

	ctl = dsp->ctl;
	in_pads = dsp->in_pads;
	out_pads = dsp->out_pads;

	for (i = 0; i<MAX_NUM_OF_OUTPUT_PADS; i++)
	{
		uint8_t pad_type;

		curr_out_pad = &out_pads[i];
		pad_type = curr_out_pad->pad_type;
		if (DSP_OUT_PAD_TYPE_NORMAL == pad_type)
		{
			curr_out_pad->buff = (real_t*)memory_pool_malloc(dsp_buffers_pool);
			curr_out_pad->buff_size =
				memory_pool_get_chunk_size(dsp_buffers_pool) / sizeof(real_t);
			curr_out_pad->sinks_processed_counter =
				curr_out_pad->total_registered_sinks;
		}
		else if (DSP_OUT_PAD_TYPE_NOT_USED == pad_type)
		{
			if (0 == dummy_output_buff_allocated)
			{
				dummy_output_buff =
						(real_t*)memory_pool_malloc(dsp_buffers_pool);
				dummy_output_buff_allocated = 1;
			}
			curr_out_pad->buff = dummy_output_buff;
			curr_out_pad->buff_size =
				memory_pool_get_chunk_size(dsp_buffers_pool) / sizeof(real_t);
		}
	}

	if (DSP_MANAGEMENT_API_MODULE_CONTROL_ON == ctl)
	{
		dsp->dsp_func(dsp, in_pads, out_pads );
	}
	else if (DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS == ctl)
	{
		for(i = 0; i < MAX_NUM_OF_OUTPUT_PADS; i++)
		{
			struct dsp_pad_t *in_pad ;
			size_t in_buf_size;

			in_pad = in_pads[i];
			in_buf_size = in_pad->buff_size;
			curr_out_pad = &out_pads[i];
			buff_size = curr_out_pad->buff_size;
			if (in_buf_size < buff_size)
			{
				buff_size = in_buf_size;
			}
			if (DSP_OUT_PAD_TYPE_NOT_USED != curr_out_pad->pad_type)
			{
				my_memcpy(curr_out_pad->buff, in_pad->buff, buff_size);
			}
		}
	}
	else // if (DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE == ctl)
	{
		for (i = 0; i < MAX_NUM_OF_OUTPUT_PADS; i++)
		{
			curr_out_pad = &out_pads[i];
			buff_size = curr_out_pad->buff_size;
			if (DSP_OUT_PAD_TYPE_NOT_USED != curr_out_pad->pad_type)
			{
				my_memset(curr_out_pad->buff, (float) 0, buff_size);
			}
		}
	}

	release_unused_buffers(in_pads);

	if ( 1 == dummy_output_buff_allocated)
	{
		memory_pool_free(dsp_buffers_pool, dummy_output_buff);
	}

}


/**
 * DSP_PROCESS_CHAIN()
 *
 * return:
 */
void DSP_PROCESS_CHAIN(struct dsp_chain_t *ap_chain)
{
	size_t i;
	struct dsp_desc_t **dsp_chain;
	dsp_chain = ap_chain->dsp_chain;
	i = ap_chain->occupied_dsp_modules;


	while (i--)
	{
		DSP_PROCESS( *dsp_chain);
		dsp_chain++;
	}

	for(i = 0; i < MAX_NUM_OF_OUTPUT_PADS; i++)
	{
		struct dsp_pad_t *curr_source_out_pad ;
//		real_t *out_buff;
		curr_source_out_pad = ap_chain->chain_out_pads[i];

		if (NULL != curr_source_out_pad)
		{
			curr_source_out_pad->buff = NULL;
			curr_source_out_pad->buff_size = 0;
		}
//		if (NULL == curr_source_out_pad)
//		{
//			continue;
//		}
//
//		out_buff = ap_chain->out_buffers[i];
//		if (NULL != out_buff)
//		{
//			my_memcpy(out_buff, curr_source_out_pad->buff, len);
//		}
//
//		if  (DSP_OUT_PAD_TYPE_NORMAL == curr_source_out_pad->pad_type)
//		{
//			if ( 0 == curr_source_out_pad->sinks_processed_counter )
//			{
//				CRITICAL_ERROR("counter already reached zero");
//			}
//			curr_source_out_pad->sinks_processed_counter--;
//			if (0 == curr_source_out_pad->sinks_processed_counter)
//			{
//				memory_pool_free(dsp_buffers_pool, curr_source_out_pad->buff);
//			}
//		}
	}
}


/**
 * DSP_CREATE_INTER_MODULES_LINK()
 *
 * return:
 */
uint8_t DSP_CREATE_INTER_MODULES_LINK(struct dsp_desc_t *src_dsp,
		DSP_OUTPUT_PADS_t src_dsp_pad, struct dsp_desc_t *sink_dsp,
		DSP_INPUT_PADS_t sink_dsp_pad)
{
	struct dsp_pad_t *p_curr_out_pad_of_source;
	struct dsp_pad_t **p_curr_in_pad_of_sink;

	p_curr_in_pad_of_sink = &sink_dsp->in_pads[sink_dsp_pad];
	if(&default_zero_buff != *p_curr_in_pad_of_sink)
	{
		CRITICAL_ERROR("sink pad already connected");
	}

	p_curr_out_pad_of_source = &(src_dsp->out_pads[src_dsp_pad]) ;
	*p_curr_in_pad_of_sink = p_curr_out_pad_of_source;

	p_curr_out_pad_of_source->pad_type = DSP_OUT_PAD_TYPE_NORMAL;
	p_curr_out_pad_of_source->total_registered_sinks++;
	return 0;
}


/**
 * DSP_CREATE_CHAIN_INPUT_TO_MODULE_LINK()
 *
 * return:
 */
uint8_t DSP_CREATE_CHAIN_INPUT_TO_MODULE_LINK(struct dsp_chain_t *ap_chain,
		DSP_INPUT_PADS_t src_dsp_pad, struct dsp_desc_t *sink_dsp,
		DSP_INPUT_PADS_t sink_dsp_pad)
{
	struct dsp_pad_t *p_curr_out_pad_of_source;
	struct dsp_pad_t **p_curr_in_pad_of_sink;

	p_curr_in_pad_of_sink = &sink_dsp->in_pads[sink_dsp_pad];
	if(&default_zero_buff != *p_curr_in_pad_of_sink)
	{
		CRITICAL_ERROR("sink pad already connected");
	}

	p_curr_out_pad_of_source = &(ap_chain->chain_in_pads[src_dsp_pad]) ;
	*p_curr_in_pad_of_sink = p_curr_out_pad_of_source;

	p_curr_out_pad_of_source->pad_type = DSP_PAD_TYPE_CHAIN_INPUT_BUFFER;
	p_curr_out_pad_of_source->total_registered_sinks++;
	return 0;

}


/**
 * DSP_CREATE_MODULE_TO_CHAIN_OUTPUT_LINK()
 *
 * return:
 */
uint8_t DSP_CREATE_MODULE_TO_CHAIN_OUTPUT_LINK(struct dsp_chain_t *ap_chain,
		DSP_OUTPUT_PADS_t sink_dsp_pad, struct dsp_desc_t *src_dsp,
		DSP_OUTPUT_PADS_t src_dsp_pad)
{
	struct dsp_pad_t *p_curr_out_pad_of_source;

	p_curr_out_pad_of_source = &(src_dsp->out_pads[src_dsp_pad]) ;
	if(DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER == p_curr_out_pad_of_source->pad_type)
	{
		CRITICAL_ERROR("source pad already connected to chain output");
	}

	ap_chain->chain_out_pads[sink_dsp_pad] = p_curr_out_pad_of_source;
	p_curr_out_pad_of_source->pad_type = DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER;
	p_curr_out_pad_of_source->total_registered_sinks++;
	return 0;
}


/**
 * DSP_SET_CHAIN_INPUT_BUFFER()
 *
 * return:
 */
void DSP_SET_CHAIN_INPUT_BUFFER(struct dsp_chain_t *ap_chain,
						DSP_INPUT_PADS_t sink_dsp_pad,
						uint8_t *buffer, size_t buff_size )
{
	struct dsp_pad_t *p_curr_out_pad_of_source;

	p_curr_out_pad_of_source = &(ap_chain->chain_in_pads[sink_dsp_pad]) ;
	p_curr_out_pad_of_source->buff = (real_t*)buffer;
	p_curr_out_pad_of_source->buff_size = buff_size;
}


/**
 * DSP_SET_CHAIN_OUTPUT_BUFFER()
 *
 * return:
 */
void DSP_SET_CHAIN_OUTPUT_BUFFER(struct dsp_chain_t *ap_chain,
						DSP_OUTPUT_PADS_t output_dsp_pad,
						uint8_t *buffer, size_t buff_size)
{
	struct dsp_pad_t *p_curr_out_pad_of_source;

	p_curr_out_pad_of_source = ap_chain->chain_out_pads[output_dsp_pad];
	if (NULL == p_curr_out_pad_of_source)
	{
		CRITICAL_ERROR("source pad not connected");
	}
	p_curr_out_pad_of_source->buff = (real_t*)buffer;
	p_curr_out_pad_of_source->buff_size = buff_size;
}


/**
 * dsp_management_api_set_module_control()
 *
 * return:
 */
void dsp_management_api_set_module_control(struct dsp_desc_t *dsp,
								dsp_management_api_module_control_t ctl)
{
	dsp->ctl = ctl ;
}


/**
 * DSP_GET_BUFFER()
 *
 * return:
 */
void DSP_GET_BUFFER(struct dsp_pad_t *pad, real_t **buff, size_t *buff_len)
{
	*buff = pad->buff;
	*buff_len = pad->buff_size;
}
