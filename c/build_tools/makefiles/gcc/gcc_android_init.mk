#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE
 
ifdef CONFIG_ANDROID_NDK_ARCH_ARM
    ARCH_PREFIX :=arm
    GCC_ARM_ARCH :=armv7-a
else
    $(error UNIMPLEMENTED ARCH)
endif

ifdef CONFIG_ANDROID_NDK_GCC_4_6
    ANDROID_GCC_VER:=4.6
else ifdef CONFIG_ANDROID_NDK_GCC_4_8
    ANDROID_GCC_VER:=4.8
else ifdef CONFIG_ANDROID_NDK_GCC_4_9
    ANDROID_GCC_VER:=4.9
else
    $(error UNSUPPORTED GCC VERSION)
endif

ARCH_DIR_NAME :=$(ARCH_PREFIX)-linux-androideabi-$(ANDROID_GCC_VER)
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    OS_DIR_NAME:=windows-x86_64
else
    OS_DIR_NAME:=linux-x86_64
endif

GNU_COMPILATION_PREFIX	:=$(ARCH_PREFIX)-linux-androideabi

ANDROID_NDK_ROOT_DIR :=

####### test for existence of microsoft compiler and put its directory name in ANDROID_NDK_ROOT_DIR #####
SEARCHED_TOOL :=$(GNU_COMPILATION_PREFIX)-gcc
SEARCHED_DIR_VARIABLE :=ANDROID_NDK_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE :=REDEFINE_ANDROID_NDK_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    DIR_NAME:=toolchains\$(ARCH_DIR_NAME)\prebuilt\$(OS_DIR_NAME)\bin
    TEST_FILE_IN_SEARCHED_DIR:=$(DIR_NAME)\$(GNU_COMPILATION_PREFIX)-gcc.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    DIR_NAME:=toolchains\$(ARCH_DIR_NAME)\prebuilt\$(OS_DIR_NAME)\bin
    TEST_FILE_IN_SEARCHED_DIR:=$(DIR_NAME)\$(GNU_COMPILATION_PREFIX)-gcc.exe
endif
include $(MAKEFILES_ROOT_DIR)/_common_include_functions/tool_existence_check.mk
####### end of tool existence test #####

GCC_ROOT_DIR :=$(ANDROID_NDK_ROOT_DIR)/toolchains
GCC_ROOT_DIR :=$(GCC_ROOT_DIR)/$(ARCH_DIR_NAME)/prebuilt/$(OS_DIR_NAME)

ifndef CONFIG_OPTIMIZE_LEVEL
    CONFIG_OPTIMIZE_LEVEL :=O0
endif



ifdef CONFIG_GCC_OPTIMISE_NONE
    CONFIG_OPTIMIZE_LEVEL := O0
else ifdef CONFIG_GCC_OPTIMISE_WITH_DEBUG
    CONFIG_OPTIMIZE_LEVEL := Og
else ifdef CONFIG_GCC_OPTIMISE_SIZE
    CONFIG_OPTIMIZE_LEVEL := Os
else ifdef CONFIG_GCC_OPTIMISE_SPEED
    CONFIG_OPTIMIZE_LEVEL := O2
else ifdef CONFIG_GCC_OPTIMISE_ULTRA_SPEED
    CONFIG_OPTIMIZE_LEVEL := O3
endif

ifdef CONFIG_CORTEX_M3
    CONFIG_CPU_TYPE := cortex-m3
else ifdef CONFIG_CORTEX_M4
    CONFIG_CPU_TYPE := cortex-m4
else ifdef CONFIG_CORTEX_A9
    CONFIG_CPU_TYPE := cortex-a9
else ifdef CONFIG_ARM_926EJ_S
    CONFIG_CPU_TYPE := arm926ej-s
endif


### GLOBAL_CFLAGS calculation

GLOBAL_CFLAGS += -mthumb
GLOBAL_CFLAGS += -fpie 
GLOBAL_CFLAGS += -fpic 
GLOBAL_CFLAGS += -fPIE 
GLOBAL_CFLAGS += -fPIC
GLOBAL_CFLAGS += -pie
GLOBAL_CFLAGS += -Wall
GLOBAL_CFLAGS += -Wno-missing-braces
GLOBAL_CFLAGS += -mword-relocations
GLOBAL_CFLAGS += -mthumb-interwork
GLOBAL_CFLAGS += -Werror
GLOBAL_CFLAGS += -g
GLOBAL_CFLAGS += -std=gnu99
GLOBAL_CFLAGS += -march=$(GCC_ARM_ARCH)

GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) #stop GLOBAL_CFLAGS calculation each time it used

### GLOBAL_ASMFLAGS calculation
GLOBAL_ASMFLAGS += -march=$(GCC_ARM_ARCH) -mthumb-interwork

GLOBAL_ASMFLAGS += -x assembler-with-cpp

GLOBAL_ASMFLAGS += -g

GLOBAL_ASMFLAGS := $(GLOBAL_ASMFLAGS)

#end of flags definitions



FULL_GCC_PREFIX 		:= $(GCC_ROOT_DIR)/bin/$(GNU_COMPILATION_PREFIX)-
#COMPILER_INCLUDE_DIR 	:= $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/include
COMPILER_INCLUDE_DIR 	:= $(ANDROID_NDK_ROOT_DIR)/platforms/android-21/arch-arm/usr/include

CC   :=	$(FULL_GCC_PREFIX)gcc -c
ASM  :=	$(FULL_GCC_PREFIX)gcc -c
LD   :=	$(FULL_GCC_PREFIX)gcc

endif
### end of section that run just one time per build
#####################################################

