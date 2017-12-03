
XCC_ROOT_DIR :=

####### test for existence of tensilica gcc compiler and  #####
####### put its directory name in XCC_ROOT_DIR     #####
SEARCHED_TOOL :=xt-xcc
SEARCHED_DIR_VARIABLE :=XCC_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE :=REDEFINE_XTENSA_XCC_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=bin\xt-xcc.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=bin/xt-xcc
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####

ifeq ("$(strip $(REDEFINE_XTENSA_CONFIGS_DIR))","")
    TMP_VAR_NAME :=$(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk
    TMP_VAR_NAME :=$(TMP_VAR_NAME)/REDEFINE_XTENSA_CONFIGS_DIR
    $(info !--- $(TMP_VAR_NAME) is empty or does not exists)
    $(error )
endif

ifeq ("$(wildcard $(REDEFINE_XTENSA_CONFIGS_DIR))","")
    TMP_VAR_NAME :=$(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk
    TMP_VAR_NAME :=$(TMP_VAR_NAME)/REDEFINE_XTENSA_CONFIGS_DIR
    $(info !--- $(REDEFINE_XTENSA_CONFIGS_DIR) does not exists)
    $(info !--- define Xtensa config dir in $(TMP_VAR_NAME))
    $(error )
endif

### GLOBAL_CFLAGS calculation

ifdef CONFIG_XCC_OPTIMISE_WITH_DEBUG
    CONFIG_OPTIMIZE_LEVEL := Og
else ifdef CONFIG_XCC_OPTIMISE_SIZE
    CONFIG_OPTIMIZE_LEVEL := Os
else ifdef CONFIG_XCC_OPTIMIZE_SPEED_SLIGHTLY
    CONFIG_OPTIMIZE_LEVEL := O1
else ifdef CONFIG_XCC_OPTIMISE_SPEED
    CONFIG_OPTIMIZE_LEVEL := O2
else ifdef CONFIG_XCC_OPTIMISE_ULTRA_SPEED
    CONFIG_OPTIMIZE_LEVEL := O3
else
    CONFIG_OPTIMIZE_LEVEL :=O0
endif

GLOBAL_CFLAGS += -$(CONFIG_OPTIMIZE_LEVEL)


GLOBAL_CFLAGS += -g


ifdef CONFIG_XCC_OPTIMISE_SIZE
    GLOBAL_CFLAGS +=  -ffunction-sections
endif

ifdef CONFIG_XTENSA_HIFI3_BD5
	XCC_CORE :=hifi3_bd5
	DUMMY := $(call ADD_TO_GLOBAL_DEFINES , PROC_hifi3_bd5)
	DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CONFIG_hifi3_bd5)
	CORE_CONFIG_DIR :=$(REDEFINE_XTENSA_CONFIGS_DIR)/hifi3_bd5/config
else
    $(info ---- unknown core)
    $(error --)
endif

ifeq ("$(wildcard $(CORE_CONFIG_DIR))","")
    $(info !--- $(CORE_CONFIG_DIR) does not exists)
    $(error )
endif

GLOBAL_CFLAGS += --xtensa-core=$(XCC_CORE)
GLOBAL_CFLAGS += --xtensa-system=$(CORE_CONFIG_DIR)


#stop GLOBAL_CFLAGS calculation each time it used:
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)

### GLOBAL_ASMFLAGS calculation

GLOBAL_ASMFLAGS := $(GLOBAL_CFLAGS) -Wa,--gdwarf-2

#end of flags definitions


#COMPILER_INCLUDE_DIR 	:= $(GCC_ROOT_DIR)/$(GNU_COMPILATION_PREFIX)/include
#COMPILER_INCLUDE_DIR 	:= $(ANDROID_NDK_ROOT_DIR)/platforms/android-21/arch-arm/usr/include

CC   :=	$(XCC_ROOT_DIR)/bin/xt-xcc -c
ASM  :=	$(XCC_ROOT_DIR)/bin/xt-xcc -c
LD   :=	$(XCC_ROOT_DIR)/bin/xt-xc++
