
include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk
ENTER_PROJECT_DIR :=
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    SHELL_CMD_DELIMITER = &
	ENTER_PROJECT_DIR += $(COMMON_PARTITION) & 
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
    SHELL_CMD_DELIMITER = ;
endif
ENTER_PROJECT_DIR += cd $(APP_ROOT_DIR) 


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 

    ERROR_LOG = $(AUTO_GENERATED_FILES_DIR)\kconfig.out

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

    KCONFIG_CMD :=$(KCONFIG_ROOT_DIR)/kconfig-mconf.exe $(COMMON_DIR)/Kconfig 2>$(ERROR_LOG)
    MANUAL_KCONFIG_CMD :=cd $(APP_ROOT_DIR) & $(COMMON_PARTITION) & $(KCONFIG_CMD)
    NEW_WIN_KCONFIG_CMD :=start /I $(KCONFIG_CMD)
    KCONFIG_PRINT_ERRORS_CMD :=type $(ERROR_LOG)

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 

    ERROR_LOG = $(AUTO_GENERATED_FILES_DIR)/kconfig.out
   
    SHELL_OUTPUT :=$(shell kconfig-mconf)

    $(shell sleep 1)
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
    
    KCONFIG_CMD :=kconfig-mconf $(COMMON_DIR)/Kconfig 2>$(ERROR_LOG)
    MANUAL_KCONFIG_CMD :=cd $(APP_ROOT_DIR) ; $(KCONFIG_CMD)
    NEW_WIN_KCONFIG_CMD :=xterm -geometry 120x40 -fa DejaVuSansMono -fs 9 -e "$(KCONFIG_CMD)"
    KCONFIG_PRINT_ERRORS_CMD :=cat $(ERROR_LOG)

endif

FILE_CONTENT := echo config COMMON_DIR_PATH>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
FILE_CONTENT += echo     string>>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
FILE_CONTENT += echo     default "$(WORKSPACE_ROOT_DIR)/common">>$(GENERATED_KCONFIG) $(SHELL_CMD_DELIMITER)
DUMMY:=$(shell $(FILE_CONTENT))


$(info running : $(NEW_WIN_KCONFIG_CMD))
SHELL_OUTPUT :=$(shell $(NEW_WIN_KCONFIG_CMD))
ifeq ($(findstring Redirection is not supported,$(SHELL_OUTPUT)),Redirection is not supported) 	 
    $(info you can run kconfig utility only from shell . open shell and run the following : )
    $(info $(ENTER_PROJECT_DIR) $(SHELL_CMD_DELIMITER) $(MAKE) menuconfig )
    $(error )
endif

SHELL_OUTPUT :=$(shell $(KCONFIG_PRINT_ERRORS_CMD))

ifneq ($(SHELL_OUTPUT),) 
    $(info        )
    $(info  $(SHELL_OUTPUT))
    $(info        )
    $(info  kconfig errors/warnings are located in $(ERROR_LOG))
    $(info        )
endif

menuconfig :
	$(info --- if kconfig window did not opened , run following command in shell : )
	$(info --- $(MANUAL_KCONFIG_CMD) )
	$(info auto generated Kconfig created)


	