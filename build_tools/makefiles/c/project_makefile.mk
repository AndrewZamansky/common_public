MAKEFLAGS += --no-builtin-rules

# test for valid target 
VALID_TARGETS := all clean rebuild export_project list_var  menuconfig
ifeq ($(filter $(MAKECMDGOALS),$(VALID_TARGETS)),)

default : 
    $(info  )
    $(info No valid targect selected . available targets listed below :)
    $(info  )
    $(info "make all"       :  build whole project)
    $(info "make clean"     :  delete objects , project outputs)
    $(info "make rebuild"   :  delete objects , project outputs and build whole project)
    $(info "make menuconfig":  create configuration file)
    $(info "make export_project"   :  export source code to archive)
    $(info "make list_var"  :  print usefull makefile variables)
    $(info  )
    $(info  )
    $(info  )

$(MAKECMDGOALS) : default
	$(info  )

# cant use $(call exit) because additional function not included yet.
DUMMY :=$(shell echo dummy )#used to delay execution of following exit:
$(error stopping ...)

endif

# continue for valid target :


# this line is reached with only COMMON_PROJECT_MAKEFILE_DIR initialized
include $(COMMON_PROJECT_MAKEFILE_DIR)/../prebuild_routines/init_workspace.mk
include $(MAKEFILES_ROOT_DIR)/common.mk
include $(PREBUILD_ROUTINES_DIR)/init_project.mk

$(call mkdir_if_not_exists, $(COMMON_PRIVATE_DIR)/c)
$(call mkdir_if_not_exists, $(OBJ_DIR))
$(call mkdir_if_not_exists, $(OUT_DIR))
$(call mkdir_if_not_exists, $(OUT_DIR_HISTORY))
$(call mkdir_if_not_exists, $(AUTO_GENERATED_FILES_DIR))
$(call mkdir_if_not_exists, $(MAKE_TEMPORARY_FILES_DIR))

PROJECT_CONFIG_H_FILE := $(AUTO_GENERATED_FILES_DIR)/project_config.h
SCAN_DIRS_FOR_CONFIG_FILES = $(APP_ROOT_DIR) $(PUBLIC_SW_PACKAGES_DIR)
SCAN_DIRS_FOR_CONFIG_FILES += $(PUBLIC_DRIVERS_DIR)

ifeq ($(strip $(CONFIG_USE_COMMON_PRIVATE_PACKAGES)),y)
    SCAN_DIRS_FOR_CONFIG_FILES += $(COMMON_PRIVATE_DIR)/c
endif

KCONFIG_PUBLIC_START_DIR :=$(COMMON_PUBLIC_DIR)/c

# if common_prvate was not selected before, then selecting it in menuconfig
# will require to restart menuconfig because correct git commit of
# common_private will be needed to checkout anyway, and all kconfig tree
# will need to be rebuilt
ifeq ($(strip $(CONFIG_USE_COMMON_PRIVATE_PACKAGES)),y)
    KCONFIG_PRIVATE_START_DIR :=$(COMMON_PRIVATE_DIR)/c
else
    KCONFIG_PRIVATE_START_DIR :=$(KCONFIG_DUMMY)
endif

GENERATE_INLUDE_COMPONENTS_MK_FILE :=\
         $(GENERATE_PRJ_FILES_DIR)/generate_include_components_mk_file.mk


menuconfig: $(PROJECT_KCONFIG)
	$(eval export KCONFIG_PUBLIC_START_DIR)
	$(eval export KCONFIG_PRIVATE_START_DIR)
	$(MAKE) -r -f $(MAKEFILES_ROOT_DIR)/menuconfig.mk


all: $(COMPONENTS_MK) $(PROJECT_CONFIG_H_FILE)
	$(MAKE) -r -f $(MAKEFILES_ROOT_DIR)/c/build.mk build_all

$(PROJECT_KCONFIG) : $(APP_ROOT_DIR)/.config
	$(eval export KCONFIG_PUBLIC_START_DIR)
	$(eval export KCONFIG_PRIVATE_START_DIR)
	$(MAKE) -r -f $(GENERATE_PRJ_FILES_DIR)/generate_project_kconfig.mk

$(COMPONENTS_MK) : $(APP_ROOT_DIR)/.config
	$(MAKE) -r -f $(GENERATE_INLUDE_COMPONENTS_MK_FILE)

$(PROJECT_CONFIG_H_FILE) : $(APP_ROOT_DIR)/.config
	$(MAKE) -r -f $(GENERATE_PRJ_FILES_DIR)/generate_project_config_h_file.mk


rebuild : clean
	$(MAKE) all


clean:
	$(RMDIR) $(OBJ_DIR)
	$(RMDIR) $(OUT_DIR)
	$(RMDIR) $(AUTO_GENERATED_FILES_DIR)


list_var:
	$(MAKE) -f $(MAKEFILES_ROOT_DIR)/list_usful_variables.mk


clean_for_export: clean
	$(call mkdir_if_not_exists, $(AUTO_GENERATED_FILES_DIR))

generate_components_mk_for_export: clean_for_export
	$(MAKE) -r -f $(GENERATE_INLUDE_COMPONENTS_MK_FILE)

export_project: generate_components_mk_for_export
	$(eval  ADDITIONAL_INIT_FILE :=$(MAKEFILES_ROOT_DIR)/c/init_for_c.mk)
	$(eval export ADDITIONAL_INIT_FILE)
	$(MAKE) -f $(MAKEFILES_ROOT_DIR)/export_project.mk export_project


.PHONY: default rebuild archive clean all $(SUBDIRS)
