
include $(MAKEFILES_ROOT_DIR)/common.mk

include .config

#testing for unique project name
ifeq ($(strip $(CONFIG_PROJECT_NAME)),)
    $(error PROJECT_NAME not found or is empty)
endif

CONFIG_PROJECT_NAME :=$(strip $(CONFIG_PROJECT_NAME))
CONFIG_PROJECT_NAME_SUFFIX :=$(strip $(CONFIG_PROJECT_NAME_SUFFIX))

# to improve speed of unique name recursive test in all subdirectoris ,
# following list of files will stop the recursivity of rwildcard function
# and files from this list will not be tested for "$(filename)/.config" pattern
UNWANTED_FILES = %zOUT %zOBJ %zOUT_history %z_auto_generated_files
UNWANTED_FILES += %project_config_includes %.config %.config.old
UNWANTED_FILES += %.gitignore %Makefile %.git %app
UNWANTED_FILES :=$(UNWANTED_FILES)

ls=$(filter-out $(UNWANTED_FILES),$(wildcard $1*))

#create entries that will open dependent component 
rwildcard=$(wildcard $1$2) $(foreach d, $(call ls,$1), $(call rwildcard,$d/,$2))
ALL_CONFIG_FILES := $(call rwildcard,$(WORKSPACE_ROOT_DIR)/apps/,.config)
ALL_CONFIG_FILES :=$(filter-out $(APP_ROOT_DIR)/.config,$(ALL_CONFIG_FILES))
ALL_CONFIG_FILES := $(strip $(ALL_CONFIG_FILES))


TEST_MK :=$(MAKEFILES_ROOT_DIR)/prebuild_routines/_prebuild_check_unique_project_name.mk
INCLUDE_ALL_CONFIG_FILES :=$(patsubst %,% $(TEST_MK),$(ALL_CONFIG_FILES))

ifeq (,$(CONFIG_PROJECT_NAME_SUFFIX))
   FULL_CURR_PROJECT_NAME :=$(CONFIG_PROJECT_NAME)
else
   FULL_CURR_PROJECT_NAME :=$(CONFIG_PROJECT_NAME)_$(CONFIG_PROJECT_NAME_SUFFIX)
endif

CONFIG_PROJECT_NAME:=dummy_1X2Y
CONFIG_PROJECT_NAME_SUFFIX:=dummy_1X2Y
include $(INCLUDE_ALL_CONFIG_FILES)

all :
	$(info prebuild check done )
