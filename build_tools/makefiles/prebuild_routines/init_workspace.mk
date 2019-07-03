#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE

# this is common_public/build_tools folder :
BUILD_TOOLS_ROOT_DIR :=$(COMMON_PROJECT_MAKEFILE_DIR)/../..
MAKEFILES_ROOT_DIR :=$(BUILD_TOOLS_ROOT_DIR)/makefiles
PREBUILD_ROUTINES_DIR :=$(MAKEFILES_ROOT_DIR)/prebuild_routines
GENERATE_PRJ_FILES_DIR :=$(MAKEFILES_ROOT_DIR)/generate_project_files

include $(PREBUILD_ROUTINES_DIR)/init_environment.mk
include $(MAKEFILES_ROOT_DIR)/common.mk


COMMON_PUBLIC_DIR =$(abspath $(BUILD_TOOLS_ROOT_DIR)/..)

TEMP :=$(realpath $(COMMON_PUBLIC_DIR)/..)
#remove prefix that appear on some machines :
PARENT_OF_COMMON_PUBLIC_DIR := $(patsubst $(APP_ROOT_DIR)/%,%,$(TEMP))

# depricated. used for older projects
WORKSPACE_ROOT_DIR :=$(PARENT_OF_COMMON_PUBLIC_DIR)



WORKSPACE_NAME := $(notdir $(abspath $(PARENT_OF_COMMON_PUBLIC_DIR)/..))

TEMP :=$(call calc_parent_dir,$(PARENT_OF_COMMON_PUBLIC_DIR))
EXTERNAL_SOURCE_ROOT_DIR :=$(TEMP)/external_source
$(call mkdir_if_not_exists, $(EXTERNAL_SOURCE_ROOT_DIR))

WORKSPACE_CONFIG :=$(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk
WORKSPACE_CONFIG_EXPORTED :=$(WORKSPACE_CONFIG).exported
ifeq ("$(wildcard $(WORKSPACE_CONFIG))","")
    ifneq ("$(wildcard $(WORKSPACE_CONFIG_EXPORTED))","")
        FILES := $(WORKSPACE_CONFIG_EXPORTED) $(WORKSPACE_CONFIG)
        ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
            FILES := $(subst /,\,$(FILES))
            DUMMY := $(shell copy /Y $(FILES))
        else
            $(info err: TODO)
            $(call exit,1)
        endif
    endif
endif
ifneq ("$(wildcard $(WORKSPACE_CONFIG))","")
    include $(WORKSPACE_CONFIG)
endif

MAKEFILES_INC_FUNC_DIR :=$(MAKEFILES_ROOT_DIR)/_include_functions
PUBLIC_DRIVERS_DIR := $(COMMON_PUBLIC_DIR)/c/hw_drivers
PUBLIC_SW_PACKAGES_DIR := $(COMMON_PUBLIC_DIR)/c/sw_packages

AUTO_GENERATED_FILES_DIR := $(APP_ROOT_DIR)/z_auto_generated_files
MAKE_TEMPORARY_FILES_DIR := $(AUTO_GENERATED_FILES_DIR)/make_tmp
OUT_DIR_HISTORY    :=    $(APP_ROOT_DIR)/zOUT_history

COMMON_PRIVATE_DIR = $(PARENT_OF_COMMON_PUBLIC_DIR)/common_private

OUT_DIR :=$(APP_ROOT_DIR)/zOUT

####### test for existence of make and put its directory name in MAKE_DIR #####
SEARCHED_TOOL:=make
SEARCHED_DIR_VARIABLE:=MAKE_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_MAKE_PROGRAM_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=bin\make.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=bin/make
endif
include $(MAKEFILES_INC_FUNC_DIR)/tool_existence_check.mk
####### end of tool existence test #####

MAKE :="$(MAKE_DIR)/bin/make"

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    #replace backslash for slash
    AUTO_GENERATED_FILES_DIR := $(subst /,\,$(AUTO_GENERATED_FILES_DIR))
    MAKE_TEMPORARY_FILES_DIR := $(subst /,\,$(MAKE_TEMPORARY_FILES_DIR))
    OUT_DIR_HISTORY := $(subst /,\,$(OUT_DIR_HISTORY))
    COMMON_PUBLIC_DIR := $(subst /,\,$(COMMON_PUBLIC_DIR))
    COMMON_PRIVATE_DIR := $(subst /,\,$(COMMON_PRIVATE_DIR))
    OUT_DIR := $(subst /,\,$(OUT_DIR))
endif


#if common_private directory dont exists then create a dummy one
$(call mkdir_if_not_exists, $(COMMON_PRIVATE_DIR))


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    SET_CC_ENV_VARS :=set APP_ROOT_DIR=$(APP_ROOT_DIR)&
    SET_CC_ENV_VARS +=set EXTERNAL_SOURCE_ROOT_DIR=$(EXTERNAL_SOURCE_ROOT_DIR)&
    SET_CC_ENV_VARS +=set PUBLIC_SW_PACKAGES_DIR=$(PUBLIC_SW_PACKAGES_DIR)&
    SET_CC_ENV_VARS +=set PUBLIC_DRIVERS_DIR=$(PUBLIC_DRIVERS_DIR)&
    SET_CC_ENV_VARS +=set COMMON_PRIVATE_DIR=$(COMMON_PRIVATE_DIR)&
else
    SET_CC_ENV_VARS :=export APP_ROOT_DIR=$(APP_ROOT_DIR);
    TMP :=$(EXTERNAL_SOURCE_ROOT_DIR)
    SET_CC_ENV_VARS +=export EXTERNAL_SOURCE_ROOT_DIR=$(TMP);
    SET_CC_ENV_VARS +=export PUBLIC_SW_PACKAGES_DIR=$(PUBLIC_SW_PACKAGES_DIR);
    SET_CC_ENV_VARS +=export PUBLIC_DRIVERS_DIR=$(PUBLIC_DRIVERS_DIR);
endif

MODIFIED_GITS :=

COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE = dummy_value

export

endif####
### end of section that run just one time per build
#####################################################
