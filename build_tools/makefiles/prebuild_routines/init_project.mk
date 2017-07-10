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
endif

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    OBJ_DIR :=$(subst /,\,$(OBJ_DIR))
    OUT_DIR :=$(subst /,\,$(OUT_DIR))
endif

COMPONENTS_MK := $(AUTO_GENERATED_FILES_DIR)/include_components.mk

include $(PREBUILD_ROUTINES_DIR)/init_versions.mk

OUTPUT_APP_NAME := out
ifdef CONFIG_OUTPUT_NAME
    OUTPUT_APP_NAME := $(CONFIG_OUTPUT_NAME)
endif

OUTPUT_CRC32 :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).crc32
