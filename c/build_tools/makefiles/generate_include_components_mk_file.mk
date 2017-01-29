
include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

ALL_CONFIG_FILES := $(call rwildcard,$(APP_ROOT_DIR)/app,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(SW_PACKAGES_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(DRIVERS_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(COMMON_PRIVATE_DIR)/,Makefile.uc.mk)
$(info scan for Makefile.uc.mk done . creating include_components.mk)


#clear file and add test for valid current location , for forcing to rebuild $(COMPONENTS_MK) after copying project
FILE_CONTENT := echo WORKSPACE_ROOT_DIR_FOR_TEST :=$(WORKSPACE_ROOT_DIR)>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
FILE_CONTENT += echo include $(MAKEFILE_DEFS_ROOT_DIR)/check_location.mk >>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
DUMMY:=$(shell $(FILE_CONTENT))

# adding "include {PATH}/Makefile.uc.mk" lines to include_components.mk
LIST_FILE_NAME_APPEND :=$(COMPONENTS_MK)
PREFIX_FOR_EACH_ITEM :=include 
SUFFIX_LINE_FOR_EACH_ITEM :=
ITEMS :=$(ALL_CONFIG_FILES)
include $(MAKEFILE_DEFS_ROOT_DIR)/_common_include_functions/add_item_list_to_file.mk
#end of file creation


$(shell echo INCLUDE_THIS_FOR_H_FILES_PATH := NO>> $(COMPONENTS_MK))


# adding following lines to include_components.mk 
# "include $(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk "
# "COMPONENT_CONFIG_FILE := {PATH}/Makefile.uc.mk "
LIST_FILE_NAME_APPEND :=$(COMPONENTS_MK)
PREFIX_FOR_EACH_ITEM := COMPONENT_CONFIG_FILE := 
SUFFIX_LINE_FOR_EACH_ITEM := include $(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk
ITEMS :=$(ALL_CONFIG_FILES)
include $(MAKEFILE_DEFS_ROOT_DIR)/_common_include_functions/add_item_list_to_file.mk
#end of file creation


all :
	$(info ---- auto generated include_components.mk created)
