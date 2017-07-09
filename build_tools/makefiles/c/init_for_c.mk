#################################################################
### the following section we need to run just one time per build
ifndef INIT_FOR_C_SECTION_THAT_SHOULD_RUN_ONCE

GLOBAL_DEFINES :=
GLOBAL_INCLUDE_DIR :=
SUBDIRS :=
LIBS :=
LIBRARIES_DIRS :=
LDS_PREPROCESSOR_FLAGS :=

ifeq ("$(wildcard .config)","")
    $(info !--- .config file dont exists . run 'make menuconfig')
    $(error )
endif

include .config

CONFIG_PROJECT_NAME :=$(strip $(CONFIG_PROJECT_NAME))
CONFIG_PROJECT_NAME :=$(patsubst "%",%,$(CONFIG_PROJECT_NAME))
CONFIG_PROJECT_NAME_SUFFIX :=$(strip $(CONFIG_PROJECT_NAME_SUFFIX))
CONFIG_PROJECT_NAME_SUFFIX :=$(patsubst "%",%,$(CONFIG_PROJECT_NAME_SUFFIX))

PROJECT_NAME :=$(CONFIG_PROJECT_NAME)
ifeq (,$(CONFIG_PROJECT_NAME_SUFFIX))
    FULL_PROJECT_NAME :=$(CONFIG_PROJECT_NAME)
else
    FULL_PROJECT_NAME :=$(CONFIG_PROJECT_NAME)_$(CONFIG_PROJECT_NAME_SUFFIX)
endif

ifeq ($(PROJECT_NAME),)      # if $(PROJECT_NAME) is empty
    $(info error : project have to be named set CONFIG_PROJECT_NAME in .config or using menuconfig)
    $(error )
endif
$(info ---- project directory : $(APP_ROOT_DIR) ---- )
$(info ---- project name as declared in .config : $(PROJECT_NAME) ---- )


$(info ---- running unique project name test ---- )
PASS_VARIABLES := MAKEFILES_ROOT_DIR=$(MAKEFILES_ROOT_DIR)
PASS_VARIABLES += PARENT_OF_COMMON_PUBLIC_DIR=$(PARENT_OF_COMMON_PUBLIC_DIR)
PASS_VARIABLES += APP_ROOT_DIR=$(APP_ROOT_DIR)
MAKE_FILE :=$(PREBUILD_ROUTINES_DIR)/prebuild_check_unique_project_name.mk 
SHELL_OUT := $(shell $(MAKE) -f $(MAKE_FILE) $(PASS_VARIABLES)  2>&1)

ifneq ($(findstring name is not unique,$(SHELL_OUT)),)#if name is not unique
    $(info )
    $(info !--- current PROJECT_NAME=$(PROJECT_NAME) , this name is already)
    $(info !--- found as project name in some other project)
    $(info !--- project name, assigned to PROJECT_NAME in .config file, should be unique)
    $(error )
endif
$(info ---- unique project name test passed ---- )

include $(PREBUILD_ROUTINES_DIR)/git_prebuild_routines.mk


ifneq ($(REDEFINE_OUTPUT_DIR),)
    OBJ_DIR :=$(REDEFINE_OUTPUT_DIR)/$(FULL_PROJECT_NAME)/zOBJ
    OUT_DIR :=$(REDEFINE_OUTPUT_DIR)/$(FULL_PROJECT_NAME)/zOUT
else
    OBJ_DIR    :=$(APP_ROOT_DIR)/zOBJ
    OUT_DIR    :=$(APP_ROOT_DIR)/zOUT    
endif

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    OBJ_DIR :=$(subst /,\,$(OBJ_DIR))
    OUT_DIR :=$(subst /,\,$(OUT_DIR))
endif

COMPONENTS_MK := $(AUTO_GENERATED_FILES_DIR)/include_components.mk
PROJECT_CONFIG_H_FILE := $(AUTO_GENERATED_FILES_DIR)/project_config.h

OUTPUT_APP_NAME := out
ifdef CONFIG_OUTPUT_NAME
    OUTPUT_APP_NAME := $(CONFIG_OUTPUT_NAME)
endif

OUTPUT_CRC32 :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).crc32



### calculate date
DATE_STR := $(subst -,$(SPACE),$(strip $(DATE)))#replace '-' with ' '
DATE_STR := $(subst /,$(SPACE),$(DATE_STR))#replace '/' with ' '

DATE_YEAR_STR :=$(word 1,$(DATE_STR))
#convert year 20XX to XX and add rest of date:
DATE_STR := $(patsubst 20%,%,$(DATE_YEAR_STR)) $(wordlist 2,3,$(DATE_STR))
DATE_STR := $(subst $(SPACE),.,$(DATE_STR))#replace ' ' with '.'

### calculating version number 
TIME_STR := $(subst :,.,$(strip $(TIME)))#replace ':' with '.'

CURR_GIT_VERSION :=$(patsubst $(PROJECT_NAME)%,%,$(CURR_GIT_BRANCH))
CURR_GIT_VERSION :=$(patsubst _%,%,$(CURR_GIT_VERSION))
ifeq ($(CURR_GIT_VERSION),) # if empty then we are on main branch     
    CURR_GIT_VERSION :=MasterV0.0
endif
MAIN_VERSION_STR := $(CURR_GIT_VERSION)
FULL_VERSION_STR := $(CURR_GIT_VERSION)r$(DATE_STR).$(TIME_STR)
GLOBAL_DEFINES += VERSION_STR="$(FULL_VERSION_STR)"

GLOBAL_CFLAGS :=
GLOBAL_ASMFLAGS :=
GLOBAL_LDFLAGS :=

ARCHIVE_OUTPUT := $(WORKSPACE_NAME).$(PROJECT_NAME).$(DATE_STR).7z

#####   global compiler and linker flags :  #####

ifdef CONFIG_DATA_LOCATION_INTERNAL_SRAM
    RAM_START_ADDR:=$(CONFIG_DT_SOC_RAM_START_ADDR)
    RAM_SIZE:=$(CONFIG_DT_SOC_RAM_SIZE)
endif
ifdef CONFIG_DATA_LOCATION_DDR
    RAM_START_ADDR:=$(CONFIG_DDR_START_ADDR)
    RAM_SIZE:=$(CONFIG_DDR_SIZE)
endif

ifdef CONFIG_CODE_LOCATION_INTERNAL_SRAM
    ROM_START_ADDR:=$(CONFIG_DT_SOC_RAM_START_ADDR)
    ROM_SIZE:=$(CONFIG_DT_SOC_RAM_SIZE)
endif
ifdef CONFIG_CODE_LOCATION_DDR
    ROM_START_ADDR:=$(CONFIG_DDR_START_ADDR)
    ROM_SIZE:=$(CONFIG_DDR_SIZE)
endif
ifdef CONFIG_CODE_LOCATION_INTERNAL_ROM
    ROM_START_ADDR:=$(CONFIG_INTERNAL_ROM_START_ADDR)
    ROM_SIZE:=$(CONFIG_INTERNAL_ROM_SIZE)
endif
ifdef CONFIG_CODE_LOCATION_INTERNAL_FLASH
    ROM_START_ADDR:=$(CONFIG_DT_SOC_FLASH_START_ADDR)
    ROM_SIZE:=$(CONFIG_DT_SOC_FLASH_SIZE)
endif
ifdef CONFIG_CODE_LOCATION_EXTERNAL_FLASH
    ROM_START_ADDR:=$(CONFIG_EXTERNAL_FLASH_START_ADDR)
    ROM_SIZE:=$(CONFIG_EXTERNAL_FLASH_SIZE)
endif

GLOBAL_DEFINES += RAM_START_ADDR=$(RAM_START_ADDR)


COMPILERS_DIR :=$(MAKEFILES_ROOT_DIR)/c/compilers
ifdef CONFIG_ARM
    ifdef CONFIG_GCC
        include $(COMPILERS_DIR)/gcc/gcc_arm_init.mk
    else ifdef CONFIG_GPP
        include $(COMPILERS_DIR)/gcc/gcc_arm_init.mk
    else ifdef CONFIG_ARMCC
        include $(COMPILERS_DIR)/armcc/armcc_init.mk
    endif
else ifdef CONFIG_PIC32
    include $(COMPILERS_DIR)/gcc/gcc_pic32_init.mk
else ifdef CONFIG_AVR
    include $(COMPILERS_DIR)/gcc/gcc_avr_init.mk
else ifdef CONFIG_STM8
    include $(COMPILERS_DIR)/cxstm8/cxstm8_init.mk
else ifdef CONFIG_HEXAGON
    include $(COMPILERS_DIR)/clang/clang_hexagon_init.mk
else ifdef CONFIG_ANDROID_NDK
    include $(COMPILERS_DIR)/gcc/gcc_android_init.mk
else ifdef CONFIG_HOST
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        ifdef CONFIG_MIN_GW_GCC
            include $(COMPILERS_DIR)/gcc/gcc_host_init.mk
        else ifdef CONFIG_MICROSOFT_COMPILER
            include $(COMPILERS_DIR)/microsoft_compiler/msvc_host_init.mk
        endif
    else
        include $(COMPILERS_DIR)/gcc/gcc_host_init.mk
    endif
else
    $(error ---- unknown compiler ----)
endif


#########################################################
#####   components configuration includes folder :  #####  
GLOBAL_INCLUDE_DIR += $(APP_ROOT_DIR)/project_config_includes
GLOBAL_INCLUDE_DIR += $(APP_ROOT_DIR)/z_auto_generated_files



INIT_FOR_C_SECTION_THAT_SHOULD_RUN_ONCE = dummy_value

export

endif####
### end of section that run just one time per build
#####################################################
