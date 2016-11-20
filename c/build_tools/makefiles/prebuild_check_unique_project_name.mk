
include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

include .config

#testing for unique project name
ifeq ($(strip $(CONFIG_PROJECT_NAME)),)
    $(error PROJECT_NAME not found or is empty)
endif

#create entries that will open dependent component 
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
ALL_PROJECT_CONFIG_FILES := $(call rwildcard,$(WORKSPACE_ROOT_DIR)/apps/,.config)
ALL_PROJECT_CONFIG_FILES :=$(filter-out $(APP_ROOT_DIR)/.config,$(ALL_PROJECT_CONFIG_FILES))
ALL_PROJECT_CONFIG_FILES := $(strip $(ALL_PROJECT_CONFIG_FILES))


CURR_PROJECT_NAME :=$(CONFIG_PROJECT_NAME)
CONFIG_PROJECT_NAME:=dummy_1X2Y
INCLUDE_ALL_PROJECT_CONFIG_FILES :=$(patsubst %,% $(MAKEFILE_DEFS_ROOT_DIR)/_prebuild_check_unique_project_name.mk ,$(ALL_PROJECT_CONFIG_FILES))

include $(INCLUDE_ALL_PROJECT_CONFIG_FILES)

all :
	$(info prebuild check done )
