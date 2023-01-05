#ifndef _U_BOOT_SHELL_CONFIG_123456_H
#define _U_BOOT_SHELL_CONFIG_123456_H


#include "project_config.h"

#include "_project.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

// put following define be after include of stdio.h
#define puts(str)   SHELL_REPLY_STR(str)

#if !defined(CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT)
	#include "dev_management_api.h"
#endif

#include "PRINTF_api.h"

#ifdef CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT
	#define run_command  run_command_lib
	#define cli_simple_run_command  cli_simple_run_command_lib
	#define cmd_process  cmd_process_lib
	#define find_cmd  find_cmd_lib
	#define run_command_repeatable  run_command_repeatable_lib
	#define run_command_list  run_command_list_lib
	#define cli_loop  cli_loop_lib
	#define cli_init  cli_init_lib
	#define cli_simple_parse_line  cli_simple_parse_line_lib
	#define cli_simple_process_macros  cli_simple_process_macros_lib
	#define cli_simple_loop  cli_simple_loop_lib
	#define cli_simple_run_command_list  cli_simple_run_command_list_lib
	#define find_cmd_tbl  find_cmd_tbl_lib
	#define cmd_usage  cmd_usage_lib
	#define _do_help  _do_help_lib
	#define do_help  do_help_lib
	#define cmd_process_error  cmd_process_error_lib
	#define console_buffer  console_buffer_lib
	#define clear_ctrlc  clear_ctrlc_lib
	#define had_ctrlc  had_ctrlc_lib
	#define ctrlc ctrlc_lib
	#define get_timer  get_timer_lib
	#define cli_readline  cli_readline_lib
	#ifndef REPLIES_REPLACE_BY_LIB_FUNCTIONS
		#define SHELL_REPLY_PRINTF  SHELL_REPLY_PRINTF_LIB
		#define SHELL_REPLY_STR  SHELL_REPLY_STR_LIB
		#define SHELL_REPLY_DATA  SHELL_REPLY_DATA_LIB
		#define REPLIES_REPLACE_BY_LIB_FUNCTIONS
	#endif
#endif


extern void SHELL_REPLY_PRINTF(const char* Format, ...);
#define printf(...)		SHELL_REPLY_PRINTF(__VA_ARGS__)

#define CONFIG_USE_STDINT
#define CONFIG_ENV_SIZE 8 // dummy

#ifndef  EXIT_SUCCESS
	typedef remove_or_rename_stdlib_h_in_uboot_include_dir  dummy_type;
#endif

//#define 	USE_HOSTCC    0
#define _LINUX_KERNEL_H
#define _LINUX_STRING_H_
#define _LINUX_TYPES_H
#define __COMPILER_H__
#define __ASM_ARM_TYPES_H
//#define __IMAGE_H__
#define __VSPRINTF_H
#define _LINUX_CTYPE_H
#define __LINUX_COMPILER_H
#define __STDLIB_H_
#define DO_DEPS_ONLY
#define  _LINUX_BITOPS_H
#define __ASM_ARM_PTRACE_H

#define CONFIG_OF_CONTROL	0

#define DONT_USE_STD_IO

#define uswap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))
#define uswap_32(x) \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | \
	 (((x) & 0x000000ff) << 24))
#define _uswap_64(x, sfx) \
	((((x) & 0xff00000000000000##sfx) >> 56) | \
	 (((x) & 0x00ff000000000000##sfx) >> 40) | \
	 (((x) & 0x0000ff0000000000##sfx) >> 24) | \
	 (((x) & 0x000000ff00000000##sfx) >>  8) | \
	 (((x) & 0x00000000ff000000##sfx) <<  8) | \
	 (((x) & 0x0000000000ff0000##sfx) << 24) | \
	 (((x) & 0x000000000000ff00##sfx) << 40) | \
	 (((x) & 0x00000000000000ff##sfx) << 56))
#if defined(__GNUC__)
# define uswap_64(x) _uswap_64(x, ull)
#else
# define uswap_64(x) _uswap_64(x, )
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
# define cpu_to_le16(x)		(x)
# define cpu_to_le32(x)		(x)
# define cpu_to_le64(x)		(x)
# define le16_to_cpu(x)		(x)
# define le32_to_cpu(x)		(x)
# define le64_to_cpu(x)		(x)
# define cpu_to_be16(x)		uswap_16(x)
# define cpu_to_be32(x)		uswap_32(x)
# define cpu_to_be64(x)		uswap_64(x)
# define be16_to_cpu(x)		uswap_16(x)
# define be32_to_cpu(x)		uswap_32(x)
# define be64_to_cpu(x)		uswap_64(x)
#else
# define cpu_to_le16(x)		uswap_16(x)
# define cpu_to_le32(x)		uswap_32(x)
# define cpu_to_le64(x)		uswap_64(x)
# define le16_to_cpu(x)		uswap_16(x)
# define le32_to_cpu(x)		uswap_32(x)
# define le64_to_cpu(x)		uswap_64(x)
# define cpu_to_be16(x)		(x)
# define cpu_to_be32(x)		(x)
# define cpu_to_be64(x)		(x)
# define be16_to_cpu(x)		(x)
# define be32_to_cpu(x)		(x)
# define be64_to_cpu(x)		(x)
#endif

#define DECLARE_GLOBAL_DATA_PTR

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__ARMCC_VERSION)
	#pragma GCC diagnostic ignored "-Wchar-subscripts"
#endif


#define __bitwise

typedef		uint64_t		__u64;
typedef		uint32_t		__u32;
typedef		uint16_t		__u16;
typedef		uint8_t			u8;
typedef		uint16_t		u16;
typedef		uint32_t		u32;
typedef		int32_t		s32;
typedef		uint64_t		u64;
typedef		uint8_t			bool;

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

#define aligned_u64 __u64 __aligned(8)
#define aligned_be64 __be64 __aligned(8)
#define aligned_le64 __le64 __aligned(8)

#define CONFIG_SPL_BUILD	1
#define __weak		 __attribute__((weak))
#define __maybe_unused

typedef unsigned long phys_addr_t;
typedef unsigned long phys_size_t;

// declare folowing structure to avoid warnings
struct udevice {
	uint8_t dummy;
};

#define false 0

#define CONFIG_IS_ENABLED(option) CONFIG##option

// dummy defines needed for compilation of u-boot sources
#define CONFIG_SYS_MAX_FLASH_SECT	1  //  "(_16MB_ / _4KB_)"
#define CONFIG_NR_DRAM_BANKS 1


#define DEBUG_PARSER 0
#define CONFIG_SYS_CBSIZE   CONFIG_UBOOT_SYS_CBSIZE
#define CONFIG_SYS_PROMPT ">"

#if defined(_WIN32) || defined(_WIN64)
	#undef _DEBUG
#endif

#ifdef DEBUG
#define _DEBUG	1
#else
#define _DEBUG	0
#endif

#if !defined(_WIN32) && !defined(_WIN64)

	#define debug_cond(cond, fmt, args...)			\
		do {						\
			if (cond)				\
				printf(pr_fmt(fmt), ##args);	\
		} while (0)

	#ifndef pr_fmt
	#define pr_fmt(fmt) fmt
	#endif

	#define debug(fmt, args...)			\
		debug_cond(_DEBUG, fmt, ##args)

	#if (defined(__ARMCC_VERSION) || \
			defined(CONFIG_XTENSA_XCC) || defined(CONFIG_XTENSA_CLANG))
		typedef unsigned long ulong;
	#endif
#else
	#define debug(fmt, args, ...)
	typedef unsigned long ulong;
#endif

extern char console_buffer[1];
void clear_ctrlc (void);
int had_ctrlc (void);
int ctrlc(void);
ulong get_timer(ulong base);
int cli_readline(const char *const prompt);

#define CMD_FLAG_REPEAT		0x0001	/* repeat last command		*/

#ifndef __COMMAND_H
	#if !defined(_WIN32) && !defined(_WIN64) && !defined(__ARMCC_VERSION)
		#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
	#endif
#endif

int run_command_repeatable(const char *cmd, int flag);

#include "common.h"

#endif
