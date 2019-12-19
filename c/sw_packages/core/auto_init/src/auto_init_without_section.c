/*
 *
 *   file  :  auto_init_with_section_bounds.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "auto_init_api.h"
#include "auto_init.h"


#include "all_auto_init_ext.c"

static struct auto_init_struct_t auto_init_table[] =
{
#include "all_auto_init.c"
};

const int auto_init_table_size =
		sizeof(auto_init_table)/sizeof(struct auto_init_struct_t);


static uint8_t auto_init_done = 0;

#ifdef CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT
void auto_init_lib_api(void)
#else
void auto_init_api(void)
#endif
{
	struct auto_init_struct_t *p_curr_auto_init;
	size_t i;

	if (auto_init_done) return;

	p_curr_auto_init = auto_init_table;

	for (i = 0; i < auto_init_table_size; i++)
	{
		auto_init_func_t init_function ;

		init_function = p_curr_auto_init->auto_init_func;
		init_function();
		p_curr_auto_init++;
	}
	auto_init_done = 1;
}
