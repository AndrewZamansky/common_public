#################################################################
### the following section we need to run just one time per build
ifndef INIT_FOR_QAIC_SECTION_THAT_SHOULD_RUN_ONCE

#change to KCONFIG_PUBLIC_START_DIR
#KCONFIG_START_DIR_PATH :=$(COMMON_PUBLIC_DIR)/qaic

ifeq ($(findstring menuconfig,$(MAKECMDGOALS)),)
    #DONT enter if we are bulding .config file now

    GLOBAL_DEFINES :=
    GLOBAL_INCLUDE_DIR :=

    include $(PREBUILD_ROUTINES_DIR)/init_project.mk

    SCAN_DIRS_FOR_CONFIG_FILES = $(APP_ROOT_DIR) $(COMMON_PUBLIC_DIR)/qaic
    SCAN_DIRS_FOR_CONFIG_FILES += $(COMMON_PRIVATE_DIR)/qaic

    #########################################################
    #####   components configuration includes folder :  #####  
    GLOBAL_INCLUDE_DIR += $(APP_ROOT_DIR)/z_auto_generated_files
    
    QAIC_ADD_SRC :=$(MAKEFILES_ROOT_DIR)/qaic/qaic_add_src.mk
endif


INIT_FOR_QAIC_SECTION_THAT_SHOULD_RUN_ONCE = dummy_value

export

endif####
### end of section that run just one time per build
#####################################################
