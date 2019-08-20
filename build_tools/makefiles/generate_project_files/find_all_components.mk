
include $(MAKEFILES_ROOT_DIR)/common.mk

# testing for unique project name
ifeq ($(strip $(CONFIG_PROJECT_NAME)),)
    $(info err: PROJECT_NAME not found or is empty)
    $(call exit,1)
endif

# to improve speed of Makefile.uc.mk recursive search in all subdirectoris ,
# following list of files will stop the recursivity of rwildcard function
# and files from this list will not be
# tested for "$(filename)/Makefile.uc.mk" pattern
UNWANTED_FILES = %zOUT %zOBJ %zOUT_history %z_auto_generated_files
UNWANTED_FILES += %project_config_includes %.config %.config.old
UNWANTED_FILES += %.gitignore %Makefile %.git %.c %.h %Makefile.uc.mk
UNWANTED_FILES :=$(UNWANTED_FILES)

ls=$(filter-out $(UNWANTED_FILES),$(wildcard $1/*))

r = $(wildcard $1/Makefile.uc.mk) $(foreach d, $(call ls,$1), $(call r,$d))
ALL_CONFIG_FILES :=$(foreach d, $(SCAN_DIRS_FOR_CONFIG_FILES), $(call r,$d))
$(info scan for Makefile.uc.mk done . creating include_components.mk)


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    GENERATE_PRJ_FILES_DIR_FOR_ECHO :=$$(GENERATE_PRJ_FILES_DIR)
    APP_ROOT_DIR_FOR_ECHO :=$$(APP_ROOT_DIR)
    PUBLIC_DRIVERS_DIR_FOR_ECHO :=$$(PUBLIC_DRIVERS_DIR)
    PUBLIC_SW_PACKAGES_DIR_FOR_ECHO :=$$(PUBLIC_SW_PACKAGES_DIR)
    COMMON_PRIVATE_DIR_FOR_ECHO :=$$(COMMON_PRIVATE_DIR)
else
    GENERATE_PRJ_FILES_DIR_FOR_ECHO :='$$(GENERATE_PRJ_FILES_DIR)'
    APP_ROOT_DIR_FOR_ECHO :='$$(APP_ROOT_DIR)'
    PUBLIC_DRIVERS_DIR_FOR_ECHO :='$$(PUBLIC_DRIVERS_DIR)'
    PUBLIC_SW_PACKAGES_DIR_FOR_ECHO :='$$(PUBLIC_SW_PACKAGES_DIR)'
    COMMON_PRIVATE_DIR_FOR_ECHO :='$$(COMMON_PRIVATE_DIR)'
endif


# replace long directory names with $(.._DIR) variables
ALL_CONFIG_FILES := $(patsubst \
                $(APP_ROOT_DIR)%,$(APP_ROOT_DIR_FOR_ECHO)%,$(ALL_CONFIG_FILES))
ALL_CONFIG_FILES := $(patsubst \
                $(PUBLIC_DRIVERS_DIR)%,\
                $(PUBLIC_DRIVERS_DIR_FOR_ECHO)%,$(ALL_CONFIG_FILES))
ALL_CONFIG_FILES := $(patsubst \
        $(PUBLIC_SW_PACKAGES_DIR)%,\
        $(PUBLIC_SW_PACKAGES_DIR_FOR_ECHO)%,$(ALL_CONFIG_FILES))
ALL_CONFIG_FILES := $(patsubst \
            $(COMMON_PRIVATE_DIR)%,\
            $(COMMON_PRIVATE_DIR_FOR_ECHO)%,$(ALL_CONFIG_FILES))


# clear file and add test for valid current location , for forcing
# to rebuild $(COMPONENTS_MK) after copying project
NEW_LINE :=WORKSPACE_ROOT_DIR_FOR_TEST :=$(PARENT_OF_COMMON_PUBLIC_DIR)
FILE_CONTENT := echo $(NEW_LINE)>$(ALL_FOUND_COMPONENTS) $(SHELL_CMD_DELIMITER)
NEW_LINE :=include $(GENERATE_PRJ_FILES_DIR_FOR_ECHO)/check_location.mk
FILE_CONTENT += echo $(NEW_LINE)>>$(ALL_FOUND_COMPONENTS) $(SHELL_CMD_DELIMITER)
DUMMY:=$(shell $(FILE_CONTENT))

# adding "include {PATH}/Makefile.uc.mk" lines to include_components.mk
LIST_FILE_NAME_APPEND :=$(ALL_FOUND_COMPONENTS)
PREFIX_FOR_EACH_ITEM :=include 
SUFFIX_LINE_FOR_EACH_ITEM :=
ITEMS :=$(ALL_CONFIG_FILES)
include $(MAKEFILES_ROOT_DIR)/_include_functions/add_item_list_to_file.mk
#end of file creation

# init INCLUDE_* variable
$(shell echo INCLUDE_THIS_COMPONENT := NO>> $(ALL_FOUND_COMPONENTS))
$(shell echo INCLUDE_THIS_FOR_H_FILES_PATH := NO>> $(ALL_FOUND_COMPONENTS))
$(shell echo INCLUDE_THIS_COMPONENT_WITHOUT_INCLUDE_PATH := NO>> $(ALL_FOUND_COMPONENTS))

ADD_COMPONENT_UCONFIG_MK :=$(GENERATE_PRJ_FILES_DIR_FOR_ECHO)/add_component_uconfig.mk

# adding following lines to include_components.mk 
# "include $(MAKEFILES_ROOT_DIR)/add_component_uconfig.mk "
# "COMPONENT_CONFIG_FILE := {PATH}/Makefile.uc.mk "
LIST_FILE_NAME_APPEND :=$(ALL_FOUND_COMPONENTS)
PREFIX_FOR_EACH_ITEM := COMPONENT_CONFIG_FILE := 
SUFFIX_LINE_FOR_EACH_ITEM := include $(ADD_COMPONENT_UCONFIG_MK)
ITEMS :=$(ALL_CONFIG_FILES)
include $(MAKEFILES_ROOT_DIR)/_include_functions/add_item_list_to_file.mk
#end of file creation
