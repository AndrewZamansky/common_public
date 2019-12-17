#include "_project.h"
#include "u-boot/include/command.h"

#include "all_cmd_ext.c"

const cmd_tbl_t u_boot_cmd_table[] =
{
#include "all_cmd.c"
};

const int u_boot_cmd_table_size = sizeof(u_boot_cmd_table)/sizeof(cmd_tbl_t);
