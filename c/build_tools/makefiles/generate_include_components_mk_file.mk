
include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

ALL_CONFIG_FILES := $(call rwildcard,$(APP_ROOT_DIR)/app,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(SW_PACKAGES_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(DRIVERS_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(COMMON_PRIVATE_DIR)/,Makefile.uc.mk)
$(info scan for uconfig.mk done )


#clear file
$(info creating include_components.mk)
DOLLAR=$
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    $(shell echo # > $(COMPONENTS_MK))
    SHELL_CMD_DELIMITER = &
    ADD_COMPONENT_UCONFIG :=echo include $$(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    $(shell echo '#' > $(COMPONENTS_MK))
    SHELL_CMD_DELIMITER = ;
    ADD_COMPONENT_UCONFIG :=echo 'include $$(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk'>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
endif

#add test for valid current location , for forcing to rebuild $(COMPONENTS_MK) after copying project
FILE_CONTENT := echo WORKSPACE_ROOT_DIR_FOR_TEST :=$(WORKSPACE_ROOT_DIR)>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
FILE_CONTENT += echo include $(MAKEFILE_DEFS_ROOT_DIR)/check_location.mk >>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
DUMMY:=$(shell $(FILE_CONTENT))

# adding "include {PATH}/Makefile.uc.mk" lines to include_components.mk 
FILE_CONTENT :=$(patsubst %, echo include %>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER),$(ALL_CONFIG_FILES))
DUMMY:=$(shell $(FILE_CONTENT))



$(shell echo INCLUDE_THIS_FOR_H_FILES_PATH := NO>> $(COMPONENTS_MK))


# adding following lines to include_components.mk 
# "include $(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk "
# "COMPONENT_CONFIG_FILE := {PATH}/Makefile.uc.mk "
FILE_CONTENT :=$(patsubst %, echo COMPONENT_CONFIG_FILE := %>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER) $(ADD_COMPONENT_UCONFIG),$(ALL_CONFIG_FILES))
DUMMY:=$(shell $(FILE_CONTENT))


all :
	$(info ---- auto generated include_components.mk created)
