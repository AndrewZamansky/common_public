/*
 *
 *   file  :  dev_placement.c
 *
 */



/***************   includes    *******************/

#include "dev_management_api.h"
#include "dev_management.h"
#include "arch.h"
#include "string.h"
#include "errors_api.h"

#ifdef CONFIG_USE_SPECIFIC_MEMORY_LOCATION_FOR_DEVICES


extern int *__start_static_devs_section ;
extern int *__stop_static_devs_section ;

struct dev_desc_t * find_static_dev_by_name(const char *device_name)
{
	struct dev_desc_t *curr_pdev;

	curr_pdev = (struct dev_desc_t *)&__start_static_devs_section;

	while((int**)curr_pdev < &__stop_static_devs_section)
	{
		uint8_t data_size;
		if((int*)DEV_MAGIC_NUMBER == curr_pdev->magic_number)
		{
			if (0 == strcmp( (char*)curr_pdev->dev_name , device_name ) )
			{
				return  curr_pdev;
			}
			data_size = sizeof(struct dev_desc_t);
		}
		else
		{
			data_size = sizeof(int*);
		}
		curr_pdev =	(struct dev_desc_t *)( ((uint8_t*)curr_pdev) + data_size);
	}
	return NULL;
}

#endif

#if (defined(CONFIG_DYNAMIC_DEVICE_TREE) ||  \
	(defined(CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES)) || \
	(CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0))

extern int *__start_modules_section ;
extern int *__stop_modules_section ;

static struct included_module_t * get_module(const char *module_name)
{
	const char *module_name_str ;
	struct included_module_t *curr_include_module;

	module_name_str = pdev->module_name;
	curr_include_module = (struct dev_desc_t *)&__start_modules_section;
	while ((int **)curr_include_module < &__stop_modules_section)
	{
		uint8_t data_size;
		if((int*)MODULE_MAGIC_NUMBER == curr_pdev->magic_number)
		{
			if (0 == strcmp(module_name_str, curr_include_module->module_name))
			{
				return curr_include_module;
			}
			data_size = sizeof(struct included_module_t);
		}
		else
		{
			/*for compilers that insert spaces between data (like MSVC)*/
			data_size = sizeof(int*);
		}
		curr_include_module =
			(struct included_module_t *)( ((uint8_t*)curr_pdev) + data_size);
	}
	CRITICAL_ERROR("module not found\n");
}
#endif

#if (defined(CONFIG_DYNAMIC_DEVICE_TREE) ||  \
	(CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0))


void init_new_device(struct dev_desc_t * pdev )
{
	struct included_module_t *module;

	module = pdev->module;

	if (NULL == pdev->p_config_data)
	{
		pdev->p_config_data = malloc(module->module_config_struct_size);
		errors_api_check_if_malloc_secceed(pdev->p_config_data);
	}
	pdev->p_runtime_data = malloc(module->module_runtime_struct_size);
	errors_api_check_if_malloc_secceed(pdev->p_runtime_data);
}
#endif

#if defined(CONFIG_DYNAMIC_DEVICE_TREE)
/*
 * function : init_device_tree()
 *
 *
 */
void init_device_tree()
{
	uint32_t *src ;
	uint32_t *dst ;
	struct dev_desc_t * curr_pdev ;
	struct dev_desc_t * start_of_dev_tree ;
	uint8_t * end_of_dev_tree ;

	src = (uint32_t *)CONFIG_DEVICE_TREE_LOCATION_ADDR;
	dst = (uint32_t *) malloc(CONFIG_DEVICE_TREE_MAXIMAL_SIZE);
	errors_api_check_if_malloc_secceed(dst);
	start_of_dev_tree = (struct dev_desc_t *)dst;
	end_of_dev_tree = (uint8_t *)dst + CONFIG_DEVICE_TREE_MAXIMAL_SIZE;
	if (src != dst)
	{
		while ((uint8_t*)dst < end_of_dev_tree)
		{
		  *dst++ = *src++; /* 4 bytes copy*/
		}
	}

	curr_pdev = start_of_dev_tree;
	while ((uint8_t*)dst < end_of_dev_tree)
	{
		uint8_t data_size;
		if((int*)DEV_MAGIC_NUMBER == curr_pdev->magic_number)
		{
			init_new_device(curr_pdev);
			data_size = sizeof(struct dev_desc_t);
		}
		else
		{
			/*for compilers that insert spaces between data (like MSVC)*/
			data_size = sizeof(int*);
		}
		curr_pdev =	(struct dev_desc_t *)( ((uint8_t*)curr_pdev) + data_size);
	}

}

AUTO_INIT_FUNCTION(init_device_tree);

#endif

#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
/*
 * function : get_config_param()
 *
 *
 */
struct dev_param_t const *get_config_param(struct dev_desc_t * pdev,
		char *param_name_str )
{
	const char *module_name_str ;
	struct included_module_t *module;
	uint8_t		 size_of_config_params_arr;
	struct dev_param_t  const *config_param;

	module_name_str = pdev->module_name;
	module = get_module(module_name_str);

	if (NULL == module) return NULL;

	config_param = curr_include_module -> config_params_arr;
	size_of_config_params_arr =	curr_include_module->size_of_config_params_arr;
	while (size_of_config_params_arr--)
	{
		if (0 == strcmp(param_name_str, config_param->paramNameStr))
		{
			return config_param;
		}
		config_param++;
	}

	return NULL;
}
#endif
