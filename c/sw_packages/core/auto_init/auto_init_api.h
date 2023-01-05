
#ifndef _auto_init_api_h_
#define _auto_init_api_h_


#include "_project_typedefs.h"
#include "_project_defines.h"

#include "arch.h"

#define  AUTO_INIT_API_VERSION    20191007


#ifdef COMPILING_FOR_HOST
	#if defined(_MSC_VER)  // microsoft compiler
		#define USE_MSVC_AUTO_INIT_PLACEMENT  1
	#elif defined(__GNUC__) // GCC compiler
		#define USE_GCC_AUTO_INIT_PLACEMENT  1
	#else
		#error "unknown host"
	#endif
#else
	#if ( defined(CONFIG_HEXAGON_COMPILER) || defined(CONFIG_ANDROID_NDK) || \
			defined(CONFIG_XTENSA_GCC) ||  \
			(defined(__GNUC__) && \
				!defined(__ARMCC_VERSION) && !defined(CONFIG_XTENSA_XCC) && \
				!defined(CONFIG_XTENSA_CLANG)) )
		#define USE_GCC_AUTO_INIT_PLACEMENT  1
	#endif
	#if defined(CONFIG_XTENSA_XCC)
		#define USE_XCC_AUTO_INIT_PLACEMENT  1
	#endif
	#if defined(CONFIG_XTENSA_CLANG)
		#define USE_CLANG_AUTO_INIT_PLACEMENT  1
	#endif
	#if defined(__ARMCC_VERSION)
		#define USE_ARMCC_AUTO_INIT_PLACEMENT  1
	#endif
#endif

#ifdef USE_MSVC_AUTO_INIT_PLACEMENT/* in case we are using microsoft compiler*/
	/* $a will be begining $c will be end of auto init functions*/
	#pragma section("init_function_object_section$b", read)
	#define AUTO_INIT_FUNCTION_PLACEMENT         \
				__declspec(dllexport) __declspec(align(8))  \
				__declspec(allocate("init_function_object_section$b"))

#elif defined(USE_GCC_AUTO_INIT_PLACEMENT)

	#define AUTO_INIT_FUNCTION_PLACEMENT    \
							__attribute__((section("auto_init_section")))

#elif defined(USE_XCC_AUTO_INIT_PLACEMENT)

	#define AUTO_INIT_FUNCTION_PLACEMENT   \
				const __attribute__((section(".rodata.keepsort._auto_init")))

#elif defined(USE_CLANG_AUTO_INIT_PLACEMENT)

	#define AUTO_INIT_FUNCTION_PLACEMENT   \
				const __attribute__((section(".rodata.keepsort._auto_init")))

#elif defined(USE_ARMCC_AUTO_INIT_PLACEMENT)

	#define AUTO_INIT_FUNCTION_PLACEMENT const  \
							__attribute__((used)) \
							__attribute__((section(".constdata.autoinit_2")))

#endif


#define AUTO_INIT_MAGIC_NUMBER	0x1A3C
typedef void (*auto_init_func_t)(void);

struct auto_init_struct_t {
	#if !defined(CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT)
		int          *magic_number;/* define field of pointer size */
	#endif
	auto_init_func_t  auto_init_func;
};


#if defined(CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT)

	#define AUTO_INIT_FUNCTION(func)    \
		struct auto_init_struct_t  auto_init_##func = { func}

#else

	#define AUTO_INIT_FUNCTION(func)   \
		extern struct auto_init_struct_t AUTO_INIT_FUNCTION_PLACEMENT \
													auto_init_##func;\
		struct auto_init_struct_t AUTO_INIT_FUNCTION_PLACEMENT \
		auto_init_##func = { (int*)AUTO_INIT_MAGIC_NUMBER , func}

#endif


void auto_init_api(void);
void auto_init_lib_api(void);

#endif
