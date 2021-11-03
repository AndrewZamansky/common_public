/*
 *
 * file :   dsp_management.c
 *
 *
 */

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
#include "errors_api.h"
}


char chain_outputs_module_name[] = "chain_outputs_module_name";
char chain_inputs_module_name[] = "chain_inputs_module_name";



static uint8_t size_of_dsp_module_type_array = 0;
static struct dsp_module_type_t *dsp_module_type_array = NULL;
static struct dsp_pad_t default_zero_buff =
							{NULL, DSP_OUT_PAD_TYPE_NOT_USED, 0, 0};
static struct dsp_pad_t dummy_output_buff =
							{NULL, DSP_OUT_PAD_TYPE_NOT_USED, 0, 0};
static	void *dsp_buffers_pool = NULL;


static struct dsp_module_inst_t * find_dsp_by_name(
		struct dsp_chain_t *p_chain , char const *module_inst_name)
{
	struct dsp_module_inst_t *module_inst_arr;
	size_t num_of_modules;

	module_inst_arr = p_chain->module_inst_arr;
	num_of_modules = p_chain->num_of_modules;

	while (num_of_modules)
	{
		if ((NULL != module_inst_arr->module_inst_name) && (0 ==
			strncmp(module_inst_arr->module_inst_name, module_inst_name, 64)))
		{
			return module_inst_arr;
		}
		module_inst_arr++;
		num_of_modules--;
	}

	return NULL;
}


uint8_t	_dsp_ioctl_2_params(chain_handle_t chain_handle,
		char const *module_name, uint8_t ioctl_num, void *param1, void *param2)
{
	struct dsp_module_inst_t *module_inst;
	struct dsp_chain_t *p_chain;

	p_chain = (struct dsp_chain_t *)chain_handle;
	if (DSP_CHAIN_STATE__READY != p_chain->chain_state)
	{
		CRITICAL_ERROR("chain is not ready");
	}

	module_inst = find_dsp_by_name( p_chain, module_name);
	if ( NULL == module_inst )
	{
		CRITICAL_ERROR("module not found");
	}
	if (DSP_MAGIC_NUMBER != module_inst->magic_num)
	{
		CRITICAL_ERROR("dsp module not initialized");
	}
	return (module_inst->module_type->ioctl)(
					module_inst, ioctl_num, param1, param2);
}


/**
 * _dsp_register_new_module_type()
 *
 * return:
 */
void _dsp_register_new_module_type(const char *module_name,
		dsp_ioctl_func_t ioctle_func, dsp_func_t dsp_func,
		bypass_func_t bypass_func, mute_func_t mute_func,
		uint32_t flags, uint16_t module_data_size)
{
	struct dsp_module_type_t* p_new_dsp_module;
	size_of_dsp_module_type_array++;
	dsp_module_type_array =
			(struct dsp_module_type_t*)os_safe_realloc(dsp_module_type_array,
			sizeof(struct dsp_module_type_t) * size_of_dsp_module_type_array);
	errors_api_check_if_malloc_succeed(dsp_module_type_array);

	if (NULL == dsp_func)
	{
		CRITICAL_ERROR("dsp_func should be defined");
	}
	if ((NULL == bypass_func) &&
			(0 == (flags & DSP_MANAGEMENT_FLAG_BYPASS_NOT_AVAILABLE)))
	{
		CRITICAL_ERROR("bypass_func should be defined");
	}
	if ((NULL == mute_func) &&
			(0 == (flags & DSP_MANAGEMENT_FLAG_MUTE_NOT_AVAILABLE)))
	{
		CRITICAL_ERROR("mute_func should be defined");
	}
	p_new_dsp_module =
			&dsp_module_type_array[size_of_dsp_module_type_array - 1];
	p_new_dsp_module->name = module_name;
	p_new_dsp_module->ioctl = ioctle_func;
	p_new_dsp_module->dsp_func = dsp_func;
	p_new_dsp_module->bypass_func = bypass_func;
	p_new_dsp_module->mute_func = mute_func;
	p_new_dsp_module->flags = flags;
	p_new_dsp_module->module_data_size = module_data_size;
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


static size_t size_of_zero_buffer;
static size_t size_of_dummy_buffer;

/**
 * dsp_management_api_init()
 *
 * return:
 */
void dsp_management_api_init(uint8_t *zero_buff, size_t a_size_of_zero_buffer,
		uint8_t *dummy_buff, size_t a_size_of_dummy_buffer)
{
	if (NULL != dsp_buffers_pool)
	{
		CRITICAL_ERROR("DSP already initialized");
	}

	size_of_zero_buffer = a_size_of_zero_buffer;
	size_of_dummy_buffer = a_size_of_dummy_buffer;
	dsp_buffers_pool = memory_pool_init();

	default_zero_buff.buff = zero_buff;
	default_zero_buff.buff_size = size_of_zero_buffer;
	default_zero_buff.pad_type = DSP_PAD_TYPE_ZERO_BUFFER;

	dummy_output_buff.buff = dummy_buff;
	dummy_output_buff.buff_size = size_of_dummy_buffer;
	dummy_output_buff.pad_type = DSP_PAD_TYPE_DUMMY_OUTPUT_BUFFER;
}


void dsp_management_api_delete(void)
{
	memory_pool_delete(dsp_buffers_pool);
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
	uint32_t alloc_size;


	pdsp_chain =
			(struct dsp_chain_t*)os_safe_malloc( sizeof(struct dsp_chain_t));
	errors_api_check_if_malloc_succeed(pdsp_chain);
	memset(pdsp_chain, 0, sizeof(struct dsp_chain_t));
	pdsp_chain->chain_state = DSP_CHAIN_STATE__INITIALIZING;

	alloc_size = num_of_modules * sizeof(struct dsp_module_inst_t);
	dsp_modules = (struct dsp_module_inst_t*)os_safe_malloc(alloc_size);
	errors_api_check_if_malloc_succeed(dsp_modules);
	pdsp_chain->module_inst_arr = dsp_modules;
	memset(dsp_modules, 0, alloc_size);
	pdsp_chain->num_of_modules = num_of_modules;
	pdsp_chain->chain_in_pads.max_used_output_num = 0;
	pdsp_chain->chain_in_pads.out_pads_arr = NULL;
	pdsp_chain->chain_out_pads.max_used_input_num = 0;
	pdsp_chain->chain_out_pads.in_pads_arr = NULL;
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
	void *handle;


	for (i = 0; i < size_of_dsp_module_type_array; i++)
	{
		module_type = &dsp_module_type_array[i];
		if (0 != strcmp(module_type_name, module_type->name))
		{
			continue;
		}

		module_inst->in_pads.max_used_input_num = 0;
		module_inst->out_pads.max_used_output_num = 0;
		module_inst->in_pads.in_pads_arr = NULL;
		module_inst->out_pads.out_pads_arr = NULL;


		module_inst->module_inst_name = module_inst_name;
		module_inst->ctl = DSP_MANAGEMENT_API_MODULE_CONTROL_ON;
		module_inst->module_type = module_type;
		handle = os_safe_malloc( module_type->module_data_size );
		errors_api_check_if_malloc_succeed(handle);
		module_inst->handle = handle;
		memset(handle, 0, module_type->module_data_size);

		module_inst->magic_num = DSP_MAGIC_NUMBER;

		retVal = (module_inst->module_type->ioctl)(
								module_inst, IOCTL_DSP_INIT, NULL, NULL);

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
		struct static_dsp_component_t arr[], uint32_t items_in_arr)
{
	uint32_t i;
	struct static_dsp_component_t *curr_chain_component;
	struct dsp_module_inst_t *curr_dsp;

	curr_chain_component = &arr[1];
	curr_dsp = &p_chain->module_inst_arr[0];
	for (i = 1; i < (items_in_arr - 1); i++)
	{
		char const *module_type_name;
		const char *module_inst_name;
		struct dsp_module_inst_t *found_module_inst;

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


static struct dsp_pad_t ** get_input_pad_to_connect(
		struct in_pads_t *in_pads, uint8_t input_pad_num)
{
	uint8_t max_used_input_num;
	struct dsp_pad_t  **in_pads_arr;

	in_pads_arr = in_pads->in_pads_arr;
	max_used_input_num = in_pads->max_used_input_num;
	if (max_used_input_num <= input_pad_num)
	{
		size_t i;

		in_pads_arr = (struct dsp_pad_t  **)os_safe_realloc(in_pads_arr,
							sizeof(struct dsp_pad_t *) * (input_pad_num + 1));
		errors_api_check_if_malloc_succeed(in_pads_arr);
		for (i = max_used_input_num; i <= input_pad_num; i++)
		{
			in_pads_arr[i] = NULL;
		}
		in_pads->in_pads_arr = in_pads_arr;
		in_pads->max_used_input_num = input_pad_num + 1;
	}
	return &in_pads_arr[input_pad_num];
}


static struct dsp_pad_t *get_output_pad_to_connect(
		struct out_pads_t *out_pads, uint8_t output_pad_num)
{
	uint8_t max_used_output_num;
	struct dsp_pad_t  *out_pads_arr;
	struct dsp_pad_t  *return_out_pad;

	out_pads_arr = out_pads->out_pads_arr;
	max_used_output_num = out_pads->max_used_output_num;
	if  (max_used_output_num <= output_pad_num)
	{
		size_t i;

		out_pads_arr = (struct dsp_pad_t  *)os_safe_realloc(out_pads_arr,
							sizeof(struct dsp_pad_t) * (output_pad_num + 1));
		errors_api_check_if_malloc_succeed(out_pads_arr);
		for (i = max_used_output_num; i <= output_pad_num; i++)
		{
			struct dsp_pad_t  *curr_out_pad;

			curr_out_pad = &out_pads_arr[i];
			curr_out_pad->pad_type = DSP_OUT_PAD_TYPE_NOT_USED;
			curr_out_pad->total_registered_sinks = 0;
			curr_out_pad->sinks_processed_counter = 0;
			curr_out_pad->buff = NULL;
			curr_out_pad->buff_size = 0;
		}
		out_pads->max_used_output_num = output_pad_num + 1;
		out_pads->out_pads_arr = out_pads_arr;
	}

	return_out_pad = &out_pads_arr[output_pad_num];
	return return_out_pad;
}

#define MAX_LEN_OF_NAME_OF_CHAIN_INPUTS   64

static void create_inter_module_link(
		struct dsp_chain_t *p_chain, struct dsp_module_inst_t *sink_dsp,
		struct static_dsp_input_pad_desc_t *input_pad_desc,
		char const *inputs_name, uint8_t run_for_allocation_only)
{
	struct dsp_pad_t **p_curr_in_pad_of_sink;
	struct dsp_pad_t *p_curr_out_pad_of_source;
	char const *src_module_inst_name;
	struct dsp_module_inst_t *src_dsp;
	uint8_t pad_type;

	while (MAX_DSP_INPUT_PAD != input_pad_desc->input_pad_num)
	{

		p_curr_in_pad_of_sink = get_input_pad_to_connect(
					&sink_dsp->in_pads, input_pad_desc->input_pad_num);

		src_module_inst_name = input_pad_desc->src_module_inst_name;
		if (NULL == src_module_inst_name)
		{
			CRITICAL_ERROR("module name should not be NULL");
		}

		if (0 == strncmp(src_module_inst_name,
				inputs_name, MAX_LEN_OF_NAME_OF_CHAIN_INPUTS))
		{
			p_curr_out_pad_of_source = get_output_pad_to_connect(
				&p_chain->chain_in_pads, input_pad_desc->src_output_pad_num);
			pad_type = DSP_PAD_TYPE_CHAIN_INPUT_BUFFER;
		}
		else
		{
			src_dsp = find_dsp_by_name( p_chain, src_module_inst_name);
			if ( NULL == src_dsp )
			{
				CRITICAL_ERROR("module not found");
			}

			p_curr_out_pad_of_source = get_output_pad_to_connect(
					&src_dsp->out_pads, input_pad_desc->src_output_pad_num);

			pad_type = p_curr_out_pad_of_source->pad_type;
			pad_type = DSP_OUT_PAD_TYPE_NORMAL;
		}

		input_pad_desc++;
		if (run_for_allocation_only)
		{
			continue;
		}

		// run following code only if NOT run_for_allocation_only
		if (NULL != *p_curr_in_pad_of_sink)
		{
			CRITICAL_ERROR("sink pad already connected");
		}
		*p_curr_in_pad_of_sink = p_curr_out_pad_of_source;
		p_curr_out_pad_of_source->total_registered_sinks++;
		p_curr_out_pad_of_source->pad_type = pad_type;
	}
}


static void create_module_to_chain_out_link( struct dsp_chain_t *p_chain,
		struct static_dsp_input_pad_desc_t *input_pad_desc,
		char const *inputs_name, uint8_t run_for_allocation_only)
{
	struct dsp_pad_t **p_curr_in_pad_of_sink;
	struct dsp_pad_t *p_curr_out_pad_of_source;
	char const *src_module_inst_name;
	struct dsp_module_inst_t *src_dsp;

	while (MAX_DSP_INPUT_PAD != input_pad_desc->input_pad_num)
	{
		src_module_inst_name = input_pad_desc->src_module_inst_name;
		if (NULL == src_module_inst_name)
		{
			CRITICAL_ERROR("module name should not be NULL");
		}

		p_curr_in_pad_of_sink = get_input_pad_to_connect(
				&p_chain->chain_out_pads, input_pad_desc->input_pad_num);

		if (0 == strncmp(src_module_inst_name,
				inputs_name, MAX_LEN_OF_NAME_OF_CHAIN_INPUTS))
		{
			p_curr_out_pad_of_source = NULL; // TODO
			// direct connection from input of chain to output of chain
			CRITICAL_ERROR("TODO : NOT YET IMPLEMENTED");
		}
		else
		{
			src_dsp = find_dsp_by_name( p_chain, src_module_inst_name);
			if ( NULL == src_dsp )
			{
				CRITICAL_ERROR("module not found");
			}
			p_curr_out_pad_of_source = get_output_pad_to_connect(
					&src_dsp->out_pads, input_pad_desc->src_output_pad_num);
		}

		input_pad_desc++;
		if (run_for_allocation_only)
		{
			continue;
		}

		// run following code only if NOT run_for_allocation_only
		if (NULL != *p_curr_in_pad_of_sink)
		{
			CRITICAL_ERROR("sink pad already connected");
		}
		*p_curr_in_pad_of_sink = p_curr_out_pad_of_source;
		p_curr_out_pad_of_source->total_registered_sinks++;
		if (DSP_OUT_PAD_TYPE_NOT_USED != p_curr_out_pad_of_source->pad_type)
		{
			// we can set only 1 link from src module pad to chain output
			// pad because before processing the chain, output buffer is set
			// into p_curr_out_pad_of_source->buff, so it can be set only 1
			// time
			CRITICAL_ERROR("source pad already used");
		}
		p_curr_out_pad_of_source->pad_type = DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER;
	}
}


static void link_modules_in_chain(struct dsp_chain_t *p_chain,
		struct static_dsp_component_t arr[], uint32_t items_in_arr,
		uint8_t run_for_allocation_only)
{
	uint32_t i;
	struct static_dsp_component_t *curr_chain_component;
	struct dsp_module_inst_t *module_inst_arr;
	struct dsp_module_inst_t *curr_dsp;
	struct static_dsp_input_pad_desc_t *input_pads_desc;
	char const *inputs_name;


	inputs_name = arr[0].module_inst_name;
	module_inst_arr = p_chain->module_inst_arr;
	curr_dsp = &module_inst_arr[0];
	curr_chain_component = &arr[1];
	for (i = 1; i < (items_in_arr - 1); i++)
	{
		input_pads_desc = curr_chain_component->input_pads_desc;

		create_inter_module_link(p_chain, curr_dsp,
				input_pads_desc, inputs_name, run_for_allocation_only);
		curr_dsp++;
		curr_chain_component++;
	}

	input_pads_desc = arr[items_in_arr - 1].input_pads_desc;
	create_module_to_chain_out_link(
			p_chain, input_pads_desc, inputs_name, run_for_allocation_only);
}


/**
 * _dsp_management_create_static_chain()
 *
 * return:
 */
chain_handle_t _dsp_management_create_static_chain(
		struct static_dsp_component_t arr[], uint32_t items_in_arr)
{
	struct dsp_chain_t *pdsp_chain;
	uint32_t i;
	uint32_t   num_of_modules;
	struct static_dsp_component_t *curr_chain_component;

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
			CHAIN_INPUTS_DSPT, sizeof(CHAIN_INPUTS_DSPT)))
	{
		CRITICAL_ERROR("first chain element must be input element");
		return NULL;
	}
	for (i = 1; i < items_in_arr - 1; i++)
	{
		const char *module_type_name;

		curr_chain_component++;
		module_type_name = curr_chain_component->module_type_name;
		if ( (0 == strncmp(module_type_name,
				CHAIN_INPUTS_DSPT, sizeof(CHAIN_INPUTS_DSPT))) ||
			(0 == strncmp(module_type_name,
				CHAIN_OUTPUTS_DSPT, sizeof(CHAIN_OUTPUTS_DSPT))) )
		{
			CRITICAL_ERROR("inputs and outputs must be first and last elements respectively");
		}
	}
	curr_chain_component++;
	if (0 != strncmp(curr_chain_component->module_type_name,
			CHAIN_OUTPUTS_DSPT, sizeof(CHAIN_OUTPUTS_DSPT)))
	{
		CRITICAL_ERROR("last chain element must be output element");
		return NULL;
	}

	num_of_modules = items_in_arr - 2;
	pdsp_chain = init_chain(num_of_modules);
	add_modules_to_chain(pdsp_chain, arr, items_in_arr);

	// run first time to allocate memories. because realloc is used.
	link_modules_in_chain(pdsp_chain, arr, items_in_arr, 1);

	// run second time to create real connections
	link_modules_in_chain(pdsp_chain, arr, items_in_arr, 0);

	//dsp_management_api_process_chain(pdsp_chain);
	pdsp_chain->chain_state = DSP_CHAIN_STATE__READY;

	return pdsp_chain;
}


/**
 * dsp_management_api_delete_chain()
 *
 * return:
 */
void dsp_management_api_delete_chain(chain_handle_t chain_handle)
{
	size_t i;
	size_t num_of_modules;
	struct dsp_module_inst_t *module_inst;
	struct dsp_chain_t *p_chain;

	p_chain = (struct dsp_chain_t *)chain_handle;
	if (DSP_CHAIN_STATE__READY != p_chain->chain_state)
	{
		CRITICAL_ERROR("chain is not ready");
	}

	num_of_modules = p_chain->num_of_modules;
	module_inst = p_chain->module_inst_arr;
	for (i = 0; i < num_of_modules; i++)
	{
		(module_inst->module_type->ioctl)(
								module_inst, IOCTL_DSP_DELETE, NULL, NULL);
		os_safe_free(module_inst->in_pads.in_pads_arr);
		os_safe_free(module_inst->out_pads.out_pads_arr);
		os_safe_free(module_inst->handle);
		module_inst++;
	}
	os_safe_free(p_chain->module_inst_arr);
	os_safe_free(p_chain);

	// delete memory pool only for deleting all DSP management
	//memory_pool_delete(dsp_buffers_pool);
}


void dsp_management_default_bypass(struct dsp_module_inst_t * adsp)
{
	uint8_t pad_type;
	size_t buff_size;
	uint8_t *in_buff;
	uint8_t *out_buff;
	struct dsp_pad_t *curr_out_pad ;
	struct dsp_pad_t *out_pads_arr ;
	struct out_pads_t   *out_pads;
	struct in_pads_t   *in_pads;
	uint8_t max_used_output_num;
	uint8_t buff_is_zero_buffer;

	// first scan of input pads
	in_pads = &adsp->in_pads;
	buff_is_zero_buffer = 1;
	for (uint8_t i = 0; i < in_pads->max_used_input_num; i++)
	{
		dsp_get_input_buffer_from_pad(
				adsp, i, &in_buff, &buff_size, &buff_is_zero_buffer);
	}

	out_pads = &adsp->out_pads;
	out_pads_arr = out_pads->out_pads_arr;
	max_used_output_num = out_pads->max_used_output_num;
	for (uint8_t i = 0; i < max_used_output_num; i++)
	{
		curr_out_pad = &out_pads_arr[i];
		pad_type = curr_out_pad->pad_type;
		if ((DSP_OUT_PAD_TYPE_NORMAL == pad_type) ||
				(DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER == pad_type))
		{
			dsp_get_input_buffer_from_pad(
					adsp, i, &in_buff, &buff_size, &buff_is_zero_buffer);
			dsp_get_output_buffer_from_pad(adsp, i, &out_buff, buff_size);
			memcpy(out_buff, in_buff, buff_size);
		}
	}
}


void dsp_management_default_mute(struct dsp_module_inst_t * adsp)
{
	uint8_t pad_type;
	size_t buff_size;
	uint8_t *in_buff;
	uint8_t *out_buff;
	struct dsp_pad_t *curr_out_pad ;
	struct dsp_pad_t *out_pads_arr ;
	struct out_pads_t   *out_pads;
	uint8_t max_used_output_num;
	struct in_pads_t  *in_pads;
	uint8_t buff_is_zero_buffer;

	// first scan of input pads
	in_pads = &adsp->in_pads;
	buff_is_zero_buffer = 1;
	for (uint8_t i = 0; i < in_pads->max_used_input_num; i++)
	{
		dsp_get_input_buffer_from_pad(
				adsp, i, &in_buff, &buff_size, &buff_is_zero_buffer);
	}

	out_pads = &adsp->out_pads;
	out_pads_arr = out_pads->out_pads_arr;
	max_used_output_num = out_pads->max_used_output_num;
	for (uint8_t i = 0; i < max_used_output_num; i++)
	{
		curr_out_pad = &out_pads_arr[i];
		pad_type = curr_out_pad->pad_type;
		if ((DSP_OUT_PAD_TYPE_NORMAL == pad_type) ||
				(DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER == pad_type))
		{
			dsp_get_input_buffer_from_pad(
					adsp, i, &in_buff, &buff_size, &buff_is_zero_buffer);
			dsp_get_output_buffer_from_pad(adsp, i, &out_buff, buff_size);
			memset(out_buff, 0, buff_size);
		}
	}
}


void release_unused_buffers(struct in_pads_t   *in_pads)
{
	uint8_t i;
	struct dsp_pad_t **in_pads_arr;
	uint8_t max_used_input_num;

	in_pads_arr = in_pads->in_pads_arr;
	max_used_input_num = in_pads->max_used_input_num;
	for (i = 0; i < max_used_input_num; i++)
	{
		struct dsp_pad_t *curr_source_out_pad ;

		curr_source_out_pad = in_pads_arr[i];

		// skip unused and not normal pads
		if ((NULL == curr_source_out_pad) ||
				(DSP_OUT_PAD_TYPE_NORMAL != curr_source_out_pad->pad_type))
		{
			continue;
		}

		if ( 0 == curr_source_out_pad->sinks_processed_counter )
		{
			CRITICAL_ERROR("counter already reached zero");
			CRITICAL_ERROR("possible cause: sink module processed before source module");
		}

		curr_source_out_pad->sinks_processed_counter--;
		if (0 == curr_source_out_pad->sinks_processed_counter)
		{
			memory_pool_free(curr_source_out_pad->buff);
			curr_source_out_pad->buff = NULL;
			curr_source_out_pad->buff_size = 0;
		}
	}
}


/**
 * dsp_process()
 *
 * return:
 */
void dsp_process(struct dsp_chain_t *p_chain, struct dsp_module_inst_t *dsp )
{
	switch(dsp->ctl)
	{
	case DSP_MANAGEMENT_API_MODULE_CONTROL_ON:
		dsp->module_type->dsp_func(dsp);
		break;
	case DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS:
		dsp->module_type->bypass_func(dsp);
		break;
	case DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE:
	default:
		dsp->module_type->mute_func(dsp);
		break;
	}

	release_unused_buffers(&dsp->in_pads);
}


/**
 * dsp_management_api_process_chain()
 *
 * return:
 */
void dsp_management_api_process_chain(chain_handle_t chain_handle)
{
	size_t i;
	struct dsp_module_inst_t *module_inst_arr;
	struct in_pads_t       *chain_out_pads;
	struct dsp_chain_t *p_chain;
	DSP_CHAIN_STATE_e       chain_state;

	p_chain = (struct dsp_chain_t *)chain_handle;
	chain_state = p_chain->chain_state;
	if ((DSP_CHAIN_STATE__READY != chain_state) &&
		(DSP_CHAIN_STATE__INITIALIZING != chain_state))// for first flush
	{
		CRITICAL_ERROR("chain is not ready");
	}
	module_inst_arr = p_chain->module_inst_arr;
	i = p_chain->num_of_modules;

	while (i--)
	{
		dsp_process(p_chain, module_inst_arr);
		module_inst_arr++;
	}

	chain_out_pads = &p_chain->chain_out_pads;
	i = chain_out_pads->max_used_input_num;
	while (i--)
	{
		struct dsp_pad_t *curr_source_out_pad ;

		curr_source_out_pad = chain_out_pads->in_pads_arr[i];

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
void dsp_management_api_set_chain_input_buffer(chain_handle_t chain_handle,
				uint16_t chain_input_pad_num, uint8_t *buffer, size_t buff_size)
{
	struct out_pads_t  *chain_in_pads;
	struct dsp_pad_t   *p_curr_out_pad_of_source;
	struct dsp_chain_t *p_chain;

	p_chain = (struct dsp_chain_t *)chain_handle;
	if (DSP_CHAIN_STATE__READY != p_chain->chain_state)
	{
		CRITICAL_ERROR("chain is not ready");
	}
	chain_in_pads = &p_chain->chain_in_pads;
	if (chain_input_pad_num < chain_in_pads->max_used_output_num)
	{
		p_curr_out_pad_of_source =
				&(chain_in_pads->out_pads_arr[chain_input_pad_num]);
		p_curr_out_pad_of_source->buff = (real_t*)buffer;
		p_curr_out_pad_of_source->buff_size = buff_size;
	}
}


/**
 * dsp_management_api_set_chain_output_buffer()
 *
 * return:
 */
void dsp_management_api_set_chain_output_buffer(chain_handle_t chain_handle,
			uint16_t chain_output_pad_num, uint8_t *buffer, size_t buff_size)
{
	struct in_pads_t       *chain_out_pads;
	struct dsp_pad_t *p_curr_out_pad_of_source;
	struct dsp_chain_t *p_chain;

	p_chain = (struct dsp_chain_t *)chain_handle;
	if (DSP_CHAIN_STATE__READY != p_chain->chain_state)
	{
		CRITICAL_ERROR("chain is not ready");
	}
	p_curr_out_pad_of_source = NULL;
	chain_out_pads = &p_chain->chain_out_pads;
	if (chain_output_pad_num < chain_out_pads->max_used_input_num)
	{
		p_curr_out_pad_of_source =
				chain_out_pads->in_pads_arr[chain_output_pad_num];
	}
	else
	{
		CRITICAL_ERROR("chain output pad not connected to any dsp module");
	}

	if (NULL == p_curr_out_pad_of_source)
	{
		CRITICAL_ERROR("chain output pad not connected to any dsp module");
	}
	p_curr_out_pad_of_source->buff = (real_t*)buffer;
	p_curr_out_pad_of_source->buff_size = buff_size;
}


/**
 * dsp_management_api_set_module_control()
 *
 * return:
 */
uint8_t dsp_management_api_set_module_control(chain_handle_t chain_handle,
		char const*module_name, enum dsp_management_api_module_control_e ctl)
{
	struct dsp_module_inst_t *module_inst;
	struct dsp_chain_t *p_chain;

	p_chain = (struct dsp_chain_t *)chain_handle;
	if (DSP_CHAIN_STATE__READY != p_chain->chain_state)
	{
		CRITICAL_ERROR("chain is not ready");
	}

	module_inst = find_dsp_by_name( p_chain, module_name);
	if ( NULL == module_inst )
	{
		CRITICAL_ERROR("module not found");
	}

	switch (ctl)
	{
	case DSP_MANAGEMENT_API_MODULE_CONTROL_BYPASS:
		if (module_inst->module_type->flags &
				DSP_MANAGEMENT_FLAG_BYPASS_NOT_AVAILABLE)
		{
			return 1; //don't continue
		}
		break;
	case DSP_MANAGEMENT_API_MODULE_CONTROL_MUTE:
		if (module_inst->module_type->flags &
				DSP_MANAGEMENT_FLAG_MUTE_NOT_AVAILABLE)
		{
			return 1; //don't continue
		}
		break;
	default:
		break;
	}
	module_inst->ctl = ctl;
	return 0;
}


/**
 * dsp_get_input_buffer_from_pad()
 *
 * return:
 */
void dsp_get_input_buffer_from_pad( struct dsp_module_inst_t * adsp,
						uint8_t pad_num, uint8_t **buff, size_t *buff_len,
						uint8_t *buff_is_zero_buffer)
{
	struct in_pads_t   *in_pads;
	struct dsp_pad_t *pad;
	size_t requsted_buff_len;
	uint8_t prev_buff_was_zero_buffer;
	uint8_t tmp_buff_is_zero_buffer;

	in_pads = &adsp->in_pads;
	if (pad_num < in_pads->max_used_input_num)
	{
		tmp_buff_is_zero_buffer = 0;
		pad = in_pads->in_pads_arr[pad_num];
		if (NULL == pad)
		{
			pad = &default_zero_buff;
			tmp_buff_is_zero_buffer = 1;
		}
		else if (NULL == pad->buff)
		{
			CRITICAL_ERROR("buffer on source dsp module was not allocated");
		}
	}
	else
	{
		pad = &default_zero_buff;
		tmp_buff_is_zero_buffer = 1;
	}

	prev_buff_was_zero_buffer = *buff_is_zero_buffer;
	if (0 == prev_buff_was_zero_buffer)
	{
		requsted_buff_len = *buff_len;
		if (pad->buff_size < requsted_buff_len)
		{
			CRITICAL_ERROR("unable to provide big enough buffer");
		}
	}
	else
	{
		// change this flag only if previous pads were zero buffers
		*buff_is_zero_buffer = tmp_buff_is_zero_buffer;
	}
	*buff = (uint8_t*)pad->buff;
	*buff_len = pad->buff_size;
}


/**
 * dsp_get_output_buffer_from_pad()
 *
 * return:
 */
void dsp_get_output_buffer_from_pad( struct dsp_module_inst_t * adsp,
						uint8_t pad_num, uint8_t **buff, size_t buff_len)
{
	struct out_pads_t   *out_pads;
	struct dsp_pad_t *pad;

	out_pads = &adsp->out_pads;
	if (pad_num < out_pads->max_used_output_num)
	{
		pad = &out_pads->out_pads_arr[pad_num];
		switch(pad->pad_type)
		{
		case DSP_OUT_PAD_TYPE_NORMAL:
			if (NULL != pad->buff)
			{
				CRITICAL_ERROR("output pad buffer already allocated");
			}
			pad->buff = (real_t*)memory_pool_malloc(
									dsp_buffers_pool, buff_len);
			pad->buff_size = buff_len;
			pad->sinks_processed_counter = pad->total_registered_sinks;
			break;
		case DSP_PAD_TYPE_CHAIN_OUTPUT_BUFFER:
			if (buff_len != pad->buff_size)
			{
				CRITICAL_ERROR("output buffer size not match requested size");
			}
			break;
		default:
			if (NULL != pad->buff)
			{
				CRITICAL_ERROR("pad buffer should not be allocated in this case");
			}
			pad = &dummy_output_buff;
			if (buff_len > pad->buff_size)
			{
				CRITICAL_ERROR("size of dummy buffer to small");
			}
			break;
		}
	}
	else
	{
		pad = &dummy_output_buff;
		if (buff_len > pad->buff_size)
		{
			CRITICAL_ERROR("size of dummy buffer to small");
		}
	}
	*buff = (uint8_t*)pad->buff;
}


void dsp_get_dummy_buffer(uint8_t **buff, size_t buff_len)
{
	if (buff_len > dummy_output_buff.buff_size)
	{
		CRITICAL_ERROR("size of dummy buffer to small");
	}
	*buff = (uint8_t*)dummy_output_buff.buff;
}
