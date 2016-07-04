
include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

ENTER_PROJECT_DIR :=
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    SHELL_CMD_DELIMITER = &
	ENTER_PROJECT_DIR += $(COMMON_PARTITION) & 
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
    SHELL_CMD_DELIMITER = ;
endif
ENTER_PROJECT_DIR += cd $(APP_ROOT_DIR) 

DUMMY:=$(shell $(RM) $(AUTO_GENERATED_FILES_DIR)/*)

FILE_CONTENT := echo config COMMON_DIR_PATH>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
FILE_CONTENT += echo     string>>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
FILE_CONTENT += echo     default "$(WORKSPACE_ROOT_DIR)/common">>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
DUMMY:=$(shell $(FILE_CONTENT))

ERROR_LOG = $(AUTO_GENERATED_FILES_DIR)/kconfig.out

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 

    ifdef REDEFINE_KCONFIG_DIR
        $(info  kconfig dir  redefined to $(REDEFINE_KCONFIG_DIR) )
        ifeq ("$(wildcard $(REDEFINE_KCONFIG_DIR))","")
            $(info kconfig path $(KCONFIG_ROOT_DIR) dont exists)
            $(info to use default kconfig location remove/comment REDEFINE_KCONFIG_DIR variable in  $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
            $(info you can set customized kconfig utility path in REDEFINE_KCONFIG_DIR variable in $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
            $(error )
        else
            KCONFIG_ROOT_DIR 	:= 	$(REDEFINE_KCONFIG_DIR)
        endif
    else
        $(info  looking for kconfig in default location)
        KCONFIG_ROOT_DIR 	:= 	$(TOOLS_ROOT_DIR)/kconfig/kconfig3.12.0
        ifeq ("$(wildcard $(KCONFIG_ROOT_DIR))","")
            $(info kconfig path $(KCONFIG_ROOT_DIR) dont exists )
            $(info download kconfig version 3.12.0 and unpack it to $(KCONFIG_ROOT_DIR)  )
            $(info make sure that file kconfig-mconf.exe is located in $(KCONFIG_ROOT_DIR)/  after unpacking   )
            $(info you can set customized kconfig utility path in REDEFINE_KCONFIG_DIR variable in $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
            $(error )
        endif
    endif

    KCONFIG_CMD :=start /I $(KCONFIG_ROOT_DIR)/kconfig-mconf.exe $(COMMON_DIR)/Kconfig 2>&1
    ifeq ($(findstring Redirection is not supported,$(SHELL_OUTPUT)),Redirection is not supported) 	 
        $(info you can run kconfig utility only from shell . open shell and run the following : )
        $(info $(ENTER_PROJECT_DIR) $(SHELL_CMD_DELIMITER) $(MAKE) menuconfig )
        $(error )
    endif

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
    SHELL_OUTPUT :=$(shell kconfig-mconf)
    ifeq ($(findstring can't find file,$(SHELL_OUTPUT)),can't find file) 
        $(info kconfig-mconf found)
    else
        $(info  $(SHELL_OUTPUT))
        $(info you need to install kconfig frontends)
        $(info example of installing in ubuntu  :)
        $(info ----------------------------------)
        $(info sudo apt-get install gperf libncurses5-dev)
        $(info cd ~;git clone https://patacongo@bitbucket.org/nuttx/tools.git tools)
        $(info cd tools/kconfig-frontends/)
        $(info ./configure --enable-mconf --disable-shared --enable-static)
        $(info make)
        $(info sudo make install )
        $(info ----------------------------------)
        $(info after installing run kconfig-mconf in shell and check that you get "can't find file ..." output only)
        $(info        )
        $(error )
    endif
    
    KCONFIG_CMD :=xterm -e "kconfig-mconf $(COMMON_DIR)/Kconfig 2>$(ERROR_LOG)"

endif

$(info running : $(KCONFIG_CMD))
SHELL_OUTPUT :=$(shell $(KCONFIG_CMD))
$(info  kconfig errors/warnings are located in $(ERROR_LOG))

menuconfig :
	$(RM) $(OBJ_DIR)
	$(RM) $(OUT_DIR)
	$(info auto generated Kconfig created)


	