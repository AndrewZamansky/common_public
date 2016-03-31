
include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

ENTER_PROJECT_DIR :=
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    SHELL_CMD_DELIMITER = &
	ENTER_PROJECT_DIR += $(COMMON_PARTITION) & 
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
    SHELL_CMD_DELIMITER = ;
endif
ENTER_PROJECT_DIR += cd $(APP_ROOT_DIR) 

FILE_CONTENT := echo config COMMON_DIR_PATH>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
FILE_CONTENT += echo     string>>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
FILE_CONTENT += echo     default "$(WORKSPACE_ROOT_DIR)/common">>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
DUMMY:=$(shell $(FILE_CONTENT))


ifdef REDEFINE_KCONFIG_DIR
    $(info  gcc dir  redefined to $(REDEFINE_KCONFIG_DIR) )
    KCONFIG_ROOT_DIR 	:= 	$(REDEFINE_KCONFIG_DIR)
else
    KCONFIG_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/kconfig/kconfig3.12.0
endif

ifeq ("$(wildcard $(KCONFIG_ROOT_DIR))","")
    $(info gcc path $(KCONFIG_ROOT_DIR) dont exists)
    $(info you can set kconfig utility path in REDEFINE_KCONFIG_DIR variable in $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
    $(error )
endif

SHELL_OUTPUT :=$(shell $(KCONFIG_ROOT_DIR)/kconfig-mconf Kconfig 2>&1)
$(info ---- $(SHELL_OUTPUT))

ifeq ($(findstring Redirection is not supported,$(SHELL_OUTPUT)),Redirection is not supported) 	 
    $(info you can run kconfig utility only from shell . open shell and run the following : )
    $(info $(ENTER_PROJECT_DIR) $(SHELL_CMD_DELIMITER) $(MAKE) menuconfig )
    $(error )
endif

menuconfig :
	$(info auto generated Kconfig created)
	$(KCONFIG_ROOT_DIR)/kconfig-mconf Kconfig


	