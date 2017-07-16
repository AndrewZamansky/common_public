
include $(MAKEFILES_ROOT_DIR)/common.mk
ENTER_PROJECT_DIR :=
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    SHELL_CMD_DELIMITER = &
	ENTER_PROJECT_DIR += $(COMMON_PARTITION) & 
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    SHELL_CMD_DELIMITER = ;
endif
ENTER_PROJECT_DIR += cd $(APP_ROOT_DIR)


####### test for existence of kconfig and put its directory name in GIT_ROOT_DIR #####
SEARCHED_TOOL:=kconfig
SEARCHED_DIR_VARIABLE:=KCONFIG_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_KCONFIG_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=kconfig-mconf.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=kconfig-mconf.exe
endif
include $(MAKEFILES_INC_FUNC_DIR)/tool_existence_check.mk
####### end of tool existence test #####

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    ERROR_LOG =  $(OUT_DIR)\kconfig.out
    KCONFIG_START_DIR_PATH := $(subst /,\,$(KCONFIG_START_DIR_PATH))


    KCONFIG_BASIC_CMD :=$(KCONFIG_ROOT_DIR)/kconfig-mconf.exe $(COMMON_PUBLIC_DIR)/Kconfig
    KCONFIG_CMD :=$(KCONFIG_BASIC_CMD) 2>$(ERROR_LOG)
    KCONFIG_CMD_FOR_WARNING_CHECK :=$(KCONFIG_BASIC_CMD) 1>$(ERROR_LOG)
    MANUAL_KCONFIG_CMD :=set "KCONFIG_START_DIR_PATH=$(KCONFIG_START_DIR_PATH)" & $(APP_PARTITION) & cd $(APP_ROOT_DIR) & $(KCONFIG_BASIC_CMD)
    MANUAL_KCONFIG_CMD_FOR_WARNING_CHECK :=set "KCONFIG_START_DIR_PATH=$(KCONFIG_START_DIR_PATH)" & $(APP_PARTITION) & cd $(APP_ROOT_DIR) & $(KCONFIG_CMD_FOR_WARNING_CHECK)
    NEW_WIN_KCONFIG_CMD :=set "KCONFIG_START_DIR_PATH=$(KCONFIG_START_DIR_PATH)" & start $(KCONFIG_CMD)
    KCONFIG_PRINT_ERRORS_CMD :=type $(ERROR_LOG)

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    ERROR_LOG =  $(OUT_DIR)/kconfig.out


##        $(info !--- $(SHELL_OUTPUT))
##        $(info !--- you need to install kconfig frontends)
##        $(info !--- example of installing in ubuntu  :)
##        $(info !--- ----------------------------------)
##        $(info !--- sudo apt-get install gperf libncurses5-dev)
##        $(info !--- cd ~;git clone https://patacongo@bitbucket.org/nuttx/tools.git tools)
##        $(info !--- cd tools/kconfig-frontends/)
##        $(info !--- ./configure --enable-mconf --disable-shared --enable-static)
##        $(info !--- make)
##        $(info !--- sudo make install )
##        $(info !--- ----------------------------------)
##        $(info !--- after installing run kconfig-mconf in shell and check that you get "can't find file ..." output only)


	$(info !--- TODO : add KCONFIG_START_DIR_PATH=$(KCONFIG_START_DIR_PATH) to shell environment)
	$(error )
    KCONFIG_CMD :=kconfig-mconf $(COMMON_PUBLIC_DIR)/Kconfig 2>$(ERROR_LOG)
    KCONFIG_CMD_FOR_WARNING_CHECK :=kconfig-mconf $(COMMON_PUBLIC_DIR)/Kconfig 1>$(ERROR_LOG)
    MANUAL_KCONFIG_CMD :=cd $(APP_ROOT_DIR) ; $(KCONFIG_CMD)
    MANUAL_KCONFIG_CMD_FOR_WARNING_CHECK :=cd $(APP_ROOT_DIR) ; $(KCONFIG_CMD_FOR_WARNING_CHECK)
    NEW_WIN_KCONFIG_CMD :=xterm -geometry 120x40 -fa DejaVuSansMono -fs 9 -e "$(KCONFIG_CMD)"
    KCONFIG_PRINT_ERRORS_CMD :=cat $(ERROR_LOG)

endif



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
	$(info --- to see kconfig warnings run following command in shell : )
	$(info --- $(MANUAL_KCONFIG_CMD_FOR_WARNING_CHECK) )
	$(info auto generated Kconfig created)