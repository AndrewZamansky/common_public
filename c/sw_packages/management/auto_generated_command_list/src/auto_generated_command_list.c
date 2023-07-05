#include "_project.h"
#include "u-boot/include/command.h"

#if defined(CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT)
	#if defined(CONFIG_INCLUDE_UBOOT_SHELL)

		#include "all_cmd_ext.c"

		const cmd_tbl_t *u_boot_cmd_table[] =
		{
		#include "all_cmd.c"
		};

		const int u_boot_cmd_table_size =
				sizeof(u_boot_cmd_table)/sizeof(cmd_tbl_t*);

	#endif
#else // if not CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT
	// binary command in library still not implemented
	#include "all_bin_commands_externals.c"

	struct shell_bin_cmd_struct_t *bin_cmd_table[] =
	{
	#include "all_bin_commands.c"
	};

	const int bin_cmd_table_size =
			sizeof(bin_cmd_table)/sizeof(struct shell_bin_cmd_struct_t *);
#endif
