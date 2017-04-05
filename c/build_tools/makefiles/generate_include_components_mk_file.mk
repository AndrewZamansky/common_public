
include $(MAKEFILES_ROOT_DIR)/common.mk

# testing for unique project name
ifeq ($(strip $(CONFIG_PROJECT_NAME)),)
    $(error PROJECT_NAME not found or is empty)
endif

# to improve speed of Makefile.uc.mk recursive search in all subdirectoris ,
# following list of files will stop the recursivity of rwildcard function
# and files from this list will not be
# tested for "$(filename)/Makefile.uc.mk" pattern
UNWANTED_FILES = %zOUT %zOBJ %zOUT_history %z_auto_generated_files
UNWANTED_FILES += %project_config_includes %.config %.config.old
UNWANTED_FILES += %.gitignore %Makefile %.git
UNWANTED_FILES :=$(UNWANTED_FILES)

ls=$(filter-out $(UNWANTED_FILES),$(wildcard $1*))

rwildcard=$(wildcard $1$2) $(foreach d, $(call ls,$1), $(call rwildcard,$d/,$2))

ALL_CONFIG_FILES := $(call rwildcard,$(APP_ROOT_DIR)/app,Makefile.uc.mk)
ALL_CONFIG_FILES += $(call rwildcard,$(SW_PACKAGES_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES += $(call rwildcard,$(DRIVERS_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES += $(call rwildcard,$(COMMON_PRIVATE_DIR)/,Makefile.uc.mk)
$(info scan for Makefile.uc.mk done . creating include_components.mk)

# replace long directory names with $(.._DIR) variables
ALL_CONFIG_FILES := $(patsubst \
                $(APP_ROOT_DIR)%,$$(APP_ROOT_DIR)%,$(ALL_CONFIG_FILES))
ALL_CONFIG_FILES := $(patsubst \
                $(DRIVERS_ROOT_DIR)%,$$(DRIVERS_ROOT_DIR)%,$(ALL_CONFIG_FILES))
ALL_CONFIG_FILES := $(patsubst \
        $(SW_PACKAGES_ROOT_DIR)%,$$(SW_PACKAGES_ROOT_DIR)%,$(ALL_CONFIG_FILES))
ALL_CONFIG_FILES := $(patsubst \
            $(COMMON_PRIVATE_DIR)%,$$(COMMON_PRIVATE_DIR)%,$(ALL_CONFIG_FILES))

# clear file and add test for valid current location , for forcing
# to rebuild $(COMPONENTS_MK) after copying project
NEW_LINE :=WORKSPACE_ROOT_DIR_FOR_TEST :=$(WORKSPACE_ROOT_DIR)
FILE_CONTENT := echo $(NEW_LINE)>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
NEW_LINE :=include $$(MAKEFILES_ROOT_DIR)/check_location.mk
FILE_CONTENT += echo $(NEW_LINE) >>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
DUMMY:=$(shell $(FILE_CONTENT))

# adding "include {PATH}/Makefile.uc.mk" lines to include_components.mk
LIST_FILE_NAME_APPEND :=$(COMPONENTS_MK)
PREFIX_FOR_EACH_ITEM :=include 
SUFFIX_LINE_FOR_EACH_ITEM :=
ITEMS :=$(ALL_CONFIG_FILES)
include $(MAKEFILES_ROOT_DIR)/_common_include_functions/add_item_list_to_file.mk
#end of file creation

# init INCLUDE_THIS_FOR_H_FILES_PATH variable
$(shell echo INCLUDE_THIS_FOR_H_FILES_PATH := NO>> $(COMPONENTS_MK))

ADD_COMPONENT_UCONFIG_MK :=$$(MAKEFILES_ROOT_DIR)/add_component_uconfig.mk

# adding following lines to include_components.mk 
# "include $(MAKEFILES_ROOT_DIR)/add_component_uconfig.mk "
# "COMPONENT_CONFIG_FILE := {PATH}/Makefile.uc.mk "
LIST_FILE_NAME_APPEND :=$(COMPONENTS_MK)
PREFIX_FOR_EACH_ITEM := COMPONENT_CONFIG_FILE := 
SUFFIX_LINE_FOR_EACH_ITEM := include $(ADD_COMPONENT_UCONFIG_MK)
ITEMS :=$(ALL_CONFIG_FILES)
include $(MAKEFILES_ROOT_DIR)/_common_include_functions/add_item_list_to_file.mk
#end of file creation


all :
	$(info ---- auto generated include_components.mk created)