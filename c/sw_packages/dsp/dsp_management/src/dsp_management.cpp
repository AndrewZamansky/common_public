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


/********  defines *********************/
#define NOT_ALLOCATED_BUFFER	255

/********  types  *********************/

/********  externals *********************/



/**********   external variables    **************/


/**********   variables for export    **************/
char chain_outputs_module_name[] = "chain_outputs_module_name";
char chain_inputs_module_name[] = "chain_inputs_module_name";


/***********   local variables    **************/

static uint8_t size_of_dsp_module_type_array = 0;
static struct dsp_module_type_t *dsp_module_type_array = NULL;
static struct dsp_pad_t default_zero_buff =
							{NULL, DSP_OUT_PAD_TYPE_NOT_USED, 0, 0};
static	void *dsp_buffers_pool = NULL;

/********************************************/


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
static void my_memset(real_t *dest ,real_t val, size_t len)
{
	while (len--)
	{
		*dest++ = val;
	}
}


static struct dsp_module_inst_t * find_dsp_by_name(
		struct dsp_chain_t *p_chain , char const *module_inst_name)
{
	struct dsp_module_inst_t *module_inst_arr;
	size_t num_of_modules;

	module_inst_arr = p_chain->module_inst_arr;
	num_of_modules = p_chain->num_of_modules;

	while (num_of_modules)
	{
		if (0 ==
			strncmp(module_inst_arr->module_inst_name, module_inst_name, 64))
		{
			return module_inst_arr;
		}
		module_inst_arr++;
		num_of_modules--;
	}

	return NULL;
}


uint8_t	dsp_management_api_ioctl_2_params(struct dsp_chain_t *p_chain,
		char const *module_name, uint8_t ioctl_num, void *param1, void *param2)
{
	struct dsp_module_inst_t *module_inst;

	module_inst = find_dsp_by_name( p_chain, module_name);
	if ( NULL == module_inst )
	{
		CRITICAL_ERROR("module not found");
	}
	if (DSP_MAGIC_NUMBER != module_inst->magic_num)
	{
		CRITICAL_ERROR("dsp module not initialized");
	}
	return (module_inst->ioctl)(module_inst, ioctl_num, param1, param2);
}

uint8_t	dsp_management_api_ioctl_1_params(struct dsp_chain_t *p_chain,
		char const *module_name, uint8_t ioctl_num, void *param1)
{
	return dsp_management_api_ioctl_2_params(
			p_chain, module_name, ioctl_num, param1, NULL);
}

uint8_t	dsp_management_api_ioctl_0_params(struct dsp_chain_t *p_chain,
		char const *module_name, uint8_t ioctl_num)
{
	return dsp_management_api_ioctl_1_params(
			p_chain, module_name, ioctl_num, NULL);
}





/**
 * _dsp_register_new_module_type()
 *
 * return:
 */
void _dsp_register_new_module_type(const char *a_module_name,
		dsp_ioctl_func_t a_ioctle_func, dsp_func_t a_dsp_func,
		uint16_t a_module_data_size)
{
	struct dsp_module_type_t* p_new_dsp_module;
	size_of_dsp_module_type_array++;
	dsp_module_type_array =
			(struct dsp_module_type_t*)os_safe_realloc(dsp_module_type_array,
			sizeof(struct dsp_module_type_t) * size_of_dsp_module_type_array);
	p_new_dsp_module =
			&dsp_module_type_array[size_of_dsp_module_type_array - 1];
	p_new_dsp_module->name = a_module_name;
	p_new_dsp_module->ioctl = a_ioctle_func;
	p_new_dsp_module->dsp_func = a_dsp_func;
	p_new_dsp_module->module_data_size = a_module_data_size;
}


/**
 * dsp_management_api_delete_modules()
 *
 * return:
 */
void dsp_management_api_delete_modules()
{
	os_safe_free(dsp_module_type_array);
	dsp_module_type_array = NULL;
}


/**
 * dsp_management_api_init()
 *
 * return:
 */
void dsp_management_api_init(size_t max_num_of_allocated_buffers,
							size_t size_of_items_in_buffer)
{
	if (NULL != dsp_buffers_pool)
	{
		CRITICAL_ERROR("DSP already initialized");
	}

	dsp_buffers_pool = memory_pool_init(max_num_of_allocated_buffers,
			size_of_items_in_buffer * sizeof(real_t));

	default_zero_buff.buff = (real_t*)memory_pool_zmalloc(dsp_buffers_pool);
	default_zero_buff.buff_size =
			memory_pool_get_chunk_size(dsp_buffers_pool) / sizeof(real_t);
	default_zero_buff.pad_type = DSP_PAD_TYPE_DUMMY_ZERO_BUFFER;
}


/**
 * init_chain()
 *
 * return:
 */
static struct dsp_chain_t *init_chain(uint32_t num_of_modules)
{
	struct dsp_chain_t *pdsp_chain;
	struct dsp_module_inst_t *dsp_modules;
	uint8_t i;
	uint32_t alloc_size;


	pdsp_chain =
			(struct dsp_chain_t*)os_safe_malloc( sizeof(struct dsp_chain_t));
	memset(pdsp_chain, 0, sizeof(struct dsp_chain_t));

	alloc_size = num_of_modules * sizeof(struct dsp_module_inst_t);
	dsp_modules = (struct dsp_module_inst_t*)os_safe_malloc(alloc_size);
	pdsp_chain->module_inst_arr = dsp_modules;
	memset(dsp_modules, 0, alloc_size);
	pdsp_chain->num_of_modules = num_of_modules;

	for (i = 0; i < MAX_NUM_OF_OUTPUT_PADS; i++)
	{
		pdsp_chain->chain_in_pads[i].pad_type =
				DSP_PAD_TYPE_CHAIN_INPUT_BUFFER;
		pdsp_chain->chain_in_pads[i].buff =	NULL;
		pdsp_chain->chain_out_pads[i] = NULL;
	}

	pdsp_chain->magic_num = DSP_MAGIC_NUMBER;
	return pdsp_chain;
}


/**
 * init_module_inst()
 *
 * return:
 */
static void init_module_inst(char const *module_type_name,
		char const *module_inst_name, struct dsp_module_inst_t *module_inst)
{
	uint8_t i;
	struct dsp_module_type_t* module_type;
	uint8_t retVal;
	struct dsp_pad_t **in_pads;
	struct dsp_pad_t *out_pads;
	struct dsp_pad_t *curr_out_pad;
	void *handle;


	for (i = 0; i < size_of_dsp_module_type_array; i++)
	{
		module_type = &dsp_module_type_array[i];
		if (0 != strcmp(module_type_name, module_type->name))
		{
			continue;
		}

		in_pads = module_inst->in_pads;
		out_pads = module_inst->out_pads;
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

		module_inst->module_inst_name = module_inst_name;
		module_inst->ctl = DSP_MANAGEMENT_API_MODULE_CONTROL_ON;
		module_inst->ioctl = module_type->ioctl;
		module_inst->dsp_func = module_type->dsp_func;
		handle = os_safe_malloc( module_type->module_data_size );
		module_inst->handle = handle;
		memset(handle, 0, module_type->module_data_size);

		module_inst->magic_num = DSP_MAGIC_NUMBER;

		retVal = (module_inst->ioctl)(module_inst, IOCTL_DSP_INIT, NULL, NULL);

		// error trap in case dsp module failed to start
		if (retVal)
		{
			CRITICAL_ERROR("dsp module failed to start");
		}

		return ;
	}

	// error trap in case module with a_module_name string was not found
	CRITICAL_ERROR("module with a_module_name string was not found");
}


static void add_modules_to_chain(struct dsp_chain_t *p_chain,
		struct static_dsp_component arr[], uint32_t items_in_arr)
{
	char const *module_type_name;
	uint32_t i;
	struct static_dsp_component *curr_chain_component;
	struct dsp_module_inst_t *curr_dsp;
	const char *module_inst_name;
	struct dsp_module_inst_t *found_module_inst;

	curr_chain_component = &arr[1];
	curr_dsp = &p_chain->module_inst_arr[0];
	for (i = 1; i < (items_in_arr - 1); i++)
	{
		module_type_name = curr_chain_component->module_type_name;
		module_inst_name = curr_chain_component->module_inst_name;
		found_module_inst = find_dsp_by_name( p_chain, module_inst_name);
		if ( NULL != found_module_inst )
		{
			CRITICAL_ERROR("module with same name already exists");
		}

		init_module_inst( module_type_name,
				curr_chain_component->module_inst_name, curr_dsp );
		curr_dsp++;
		curr_chain_component++;
	}
}





static void create_inter_module_link(
		struct dsp_chain_t *p_chain, struct dsp_module_inst_t *sink_dsp,
		struct static_dsp_source_pad source_pads[], char const *inputs_name)
{
	char const *src_module_inst_name;
	uint8_t src_output_pad_num;
	uint8_t input_pad_num;
	struct static_dsp_source_pad *source_pad;
	struct dsp_module_inst_t *src_dsp;
	uint8_t pad_type;

	for (input_pad_num = 0;
			input_pad_num < MAX_NUM_OF_OUTPUT_PADS; input_pad_num++)
	{
		struct dsp_pad_t **p_curr_in_pad_of_sink;
		struct dsp_pad_t *p_curr_out_pad_of_source;

		source_pad = &source_pads[input_pad_num];
		src_module_inst_name = source_pad->src_module_inst_name;
		if (NULL == src_module_inst_name)
		{
			continue;
		}
		src_output_pad_num = source_pad->src_output_pad_num;

		p_curr_in_pad_of_sink = &sink_dsp->in_pads[input_pad_num];
		if (&default_zero_buff != *p_curr_in_pad_of_sink)
		{
			CRITICAL_ERROR("sink pad already connected");
		}

		if (0 == strncmp(src_module_inst_name, inputs_name, 64))
		{
			p_curr_in_pad_of_sink = &sink_dsp->in_pads[src_output_pad_num];

			p_curr_out_pad_of_source =
					&(p_chain->chain_in_pads[src_output_pad_num]) ;
			*p_curr_in_pad_of_sink = p_curr_out_pad_of_source;

			p_curr_out_pad_of_source->pad_type =
					DSP_PAD_TYPE_CHAIN_INPUT_BUFFER;
			p_curr_out_pad_of_source->total_registered_sinks++;
		}
		else
		{
			src_dsp = find_dsp_by_name( p_chain, src_module_inst_name);
			if ( NULL == src_dsp )
			{
				CRITICAL_ERROR("module not found");
			}

			p_curr_out_pad_of_source = &(src_dsp->out_pads[src_output_pad_num]);
			*p_curr_in_pad_of_sink = p_curr_out_pad_of_source;

			pad_type = p_curr_out_pad_of_source->pad_type;
			if ( (DSP_OUT_PAD_TYPE_NOT_USED != pad_type) &&
					(DSP_OUT_PAD_TYPE_NORMAL != pad_type) )
			{
				CRITICAL_ERROR("source pad already used");
				//CRITICAL_ERROR("src pad already connected to chain output");
			}

			p_curr_out_pad_of_source->pad_type = DSP_OUT_PAD_TYPE_NORMAL;
			p_curr_out_pad_of_source->total_registered_sinks++;
		}
	}
}


static void create_module_to_chain_out_link( struct dsp_chain_t *p_chain,
		struct static_dsp_source_pad source_pads[], char const *inputs_name)
{
	char const *src_module_inst_name;
	uint8_t src_output_pad_num;
	uint8_t input_pad_num;
	struct static_dsp_source_pad *source_pad;
	struct dsp_module_inst_t *src_dsp;

	for (input_pad_num = 0;
			input_pad_num < MAX_NUM_OF_OUTPUT_PADS; input_pad_num++)
	{
		struct dsp_pad_t *p_curr_out_pad_of_source;

		source_pad = &source_pads[input_pad_num];
		src_module_inst_name = source_pad->src_module_inst_name;
		if (NULL == src_module_inst_name)
		{
			continue;
		}
		src_output_pad_num = source_pad->src_output_pad_num;


		if (0 == strncmp(src_module_inst_name, inputs_name, 64))
		{
			CRITICAL_ERROR("TODO : NOT YET IMPLEMENTED");

			p_curr_out_pad_of_source =
					&(p_chain->chain_in_pads[src_output_pad_num]) ;
			p_chain->chain_out_pads[input_pad_num] = p_curr_out_pad_of_source;

			p_curr_out_pad_of_source->pad_type =
					DSP_PAD_TYPE_CHAIN_INPUT_BUFFER;
			p_curr_out_pad_of_source->total_registered_sinks++;
		}
		else
		{
			src_dsp = find_dsp_by_name( p_chain, src_module_inst_name);
			if ( NULL == src_dsp )
			{
				CRITICAL_ERROR("module not found");
			}

			p_curr_out_pad_of_source = &(src_dsp->out_pads[src_output_pad_num]);
			if (DSP_OUT_PAD_TYPE_NOT_USED !=
					p_curr_out_pad_of_source->pad_type)
			{
				CRITICAL_ERROR("source pad already used");
				//CRITICAL_ERROR("src pad already connected to chain output");
			}

			p_chain->chain_out_pads[input_pad_num] = p_curr_out_pad_of_source;
			p_curr_out_pad_of_source->pad_type =
					DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER;
			p_curr_out_pad_of_source->total_registered_sinks++;
		}
	}
}


static void link_modules_in_chain(struct dsp_chain_t *p_chain,
		struct static_dsp_component arr[], uint32_t items_in_arr)
{
	uint32_t i;
	struct static_dsp_component *curr_chain_component;
	struct dsp_module_inst_t *module_inst_arr;
	struct dsp_module_inst_t *curr_dsp;
	struct static_dsp_source_pad *source_pads;
	char const *inputs_name;


	inputs_name = arr[0].module_inst_name;
	module_inst_arr = p_chain->module_inst_arr;
	curr_dsp = &module_inst_arr[0];
	curr_chain_component = &arr[1];
	for (i = 1; i < (items_in_arr - 1); i++)
	{
		source_pads = curr_chain_component->source_pads;

		create_inter_module_link(p_chain, curr_dsp,
				source_pads, inputs_name);
		curr_dsp++;
		curr_chain_component++;
	}

	source_pads = arr[items_in_arr - 1].source_pads;
	create_module_to_chain_out_link(p_chain, source_pads, inputs_name);
}


/**
 * _dsp_management_create_static_chain()
 *
 * return:
 */
struct dsp_chain_t *_dsp_management_create_static_chain(
		struct static_dsp_component arr[], uint32_t items_in_arr)
{
	struct dsp_chain_t *pdsp_chain;
	uint32_t i;
	uint32_t   num_of_modules;
	struct static_dsp_component *curr_chain_component;
	const char *module_type_name;

	if (NULL == dsp_buffers_pool)
	{
		CRITICAL_ERROR("DSP not initialized");
		return NULL;
	}

	if (2 > items_in_arr)
	{
		CRITICAL_ERROR("chain should contain input and output elements");
		return NULL;
	}

	curr_chain_component = &arr[0];
	if (0 != strncmp(curr_chain_component->module_type_name,
			CHAIN_INPUTS_MODULE_NAME, sizeof(CHAIN_INPUTS_MODULE_NAME)))
	{
		CRITICAL_ERROR("first chain element must be input element");
		return NULL;
	}
	for (i = 1; i < items_in_arr - 1; i++)
	{
		curr_chain_component++;
		module_type_name = curr_chain_component->module_type_name;
		if ( (0 == strncmp(module_type_name,
				CHAIN_INPUTS_MODULE_NAME, sizeof(CHAIN_INPUTS_MODULE_NAME))) ||
			(0 == strncmp(module_type_name,
				CHAIN_OUTPUTS_MODULE_NAME, sizeof(CHAIN_OUTPUTS_MODULE_NAME))) )
		{
			CRITICAL_ERROR("inputs and outputs must be first and last elements respectively");
		}
	}
	curr_chain_component++;
	if (0 != strncmp(curr_chain_component->module_type_name,
			CHAIN_OUTPUTS_MODULE_NAME, sizeof(CHAIN_OUTPUTS_MODULE_NAME)))
	{
		CRITICAL_ERROR("last chain element must be output element");
		return NULL;
	}

	num_of_modules = items_in_arr - 2;
	pdsp_chain = init_chain(num_of_modules);
	add_modules_to_chain(pdsp_chain, arr, items_in_arr);
	link_modules_in_chain(pdsp_chain, arr, items_in_arr);

	return pdsp_chain;
}


/**
 * dsp_management_api_delete_chain()
 *
 * return:
 */
void dsp_management_api_delete_chain(struct dsp_chain_t * ap_chain)
{
	size_t i;
	size_t num_of_modules;
	struct dsp_module_inst_t *module_inst;

	num_of_modules = ap_chain->num_of_modules;
	module_inst = ap_chain->module_inst_arr;
	for (i=0 ; i < num_of_modules ; i++)
	{
		(module_inst->ioctl)(module_inst, IOCTL_DSP_DELETE, NULL, NULL);
		os_safe_free(module_inst->handle);
		module_inst++;
	}
	os_safe_free(ap_chain->module_inst_arr);
	os_safe_free(ap_chain);
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
				CRITICAL_ERROR("possible cause: sink module processed before source module");
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
 * dsp_process()
 *
 * return:
 */
void dsp_process(struct dsp_module_inst_t *dsp )
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
 * dsp_management_api_process_chain()
 *
 * return:
 */
void dsp_management_api_process_chain(struct dsp_chain_t *ap_chain)
{
	size_t i;
	struct dsp_module_inst_t *module_inst_arr;

	module_inst_arr = ap_chain->module_inst_arr;
	i = ap_chain->num_of_modules;

	while (i--)
	{
		dsp_process( module_inst_arr);
		module_inst_arr++;
	}

	for(i = 0; i < MAX_NUM_OF_OUTPUT_PADS; i++)
	{
		struct dsp_pad_t *curr_source_out_pad ;

		curr_source_out_pad = ap_chain->chain_out_pads[i];

		if (NULL != curr_source_out_pad)
		{
			curr_source_out_pad->buff = NULL;
			curr_source_out_pad->buff_size = 0;
		}
	}
}



/**
 * dsp_management_api_set_chain_input_buffer()
 *
 * return:
 */
void dsp_management_api_set_chain_input_buffer(struct dsp_chain_t *ap_chain,
						DSP_INPUT_PADS_t sink_dsp_pad,
						uint8_t *buffer, size_t buff_size )
{
	struct dsp_pad_t *p_curr_out_pad_of_source;

	p_curr_out_pad_of_source = &(ap_chain->chain_in_pads[sink_dsp_pad]) ;
	p_curr_out_pad_of_source->buff = (real_t*)buffer;
	p_curr_out_pad_of_source->buff_size = buff_size;
}


/**
 * dsp_management_api_set_chain_output_buffer()
 *
 * return:
 */
void dsp_management_api_set_chain_output_buffer(struct dsp_chain_t *ap_chain,
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
void dsp_management_api_set_module_control(
		struct dsp_chain_t *p_chain,
		char const*module_name,
		dsp_management_api_module_control_t ctl)
{
	struct dsp_module_inst_t *module_inst;

	module_inst = find_dsp_by_name( p_chain, module_name);
	if ( NULL == module_inst )
	{
		CRITICAL_ERROR("module not found");
	}

	module_inst->ctl = ctl ;
}


/**
 * dsp_get_buffer_from_pad()
 *
 * return:
 */
void dsp_get_buffer_from_pad(struct dsp_pad_t *pad, real_t **buff, size_t *buff_len)
{
	*buff = pad->buff;
	*buff_len = pad->buff_size;
}



