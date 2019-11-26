#include "_project.h"
#include "u-boot/include/command.h"

int do_get_version (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

cmd_tbl_t u_boot_cmd_table[] =
{
#include "cmd.list"
};

const int u_boot_cmd_table_size = sizeof(u_boot_cmd_table)/sizeof(cmd_tbl_t);
