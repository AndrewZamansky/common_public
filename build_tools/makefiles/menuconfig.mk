
include $(MAKEFILES_ROOT_DIR)/common.mk
ENTER_PROJECT_DIR :=
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ENTER_PROJECT_DIR += $(COMMON_PARTITION) & 
endif
ENTER_PROJECT_DIR += cd $(APP_ROOT_DIR)


### test for existence of kconfig and put its directory name in GIT_ROOT_DIR ###
SEARCHED_TOOL:=kconfig
SEARCHED_DIR_VARIABLE:=KCONFIG_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_KCONFIG_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=kconfig-mconf.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=kconfig-mconf
endif
include $(MAKEFILES_INC_FUNC_DIR)/tool_existence_check.mk
####### end of tool existence test #####

MAIN_KCONFIG :=$(COMMON_PUBLIC_DIR)/Kconfig


# if no Kconfig in "common_priveate" then create dummy one
COMMON_PRIVATE_KCONFIG_FILE :=$(KCONFIG_PRIVATE_START_DIR)/Kconfig
ifeq ("$(wildcard $(COMMON_PRIVATE_KCONFIG_FILE))","")
    $(call mkdir_if_not_exists, $(dir $(COMMON_PRIVATE_KCONFIG_FILE)))
    DUMMY:=$(shell echo config PRIVATE_DUMMY>$(COMMON_PRIVATE_KCONFIG_FILE))
endif


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    ERROR_LOG =  $(OUT_DIR)\kconfig.out
    KCONFIG_START_DIR_PATH := $(subst /,\,$(KCONFIG_START_DIR_PATH))

    SET_ENV_CMD :=set "KCONFIG_PUBLIC_START_DIR=$(KCONFIG_PUBLIC_START_DIR)" &
    SET_ENV_CMD +=set "KCONFIG_PRIVATE_START_DIR=$(KCONFIG_PRIVATE_START_DIR)"
    CHNG_DIR_CMD :=cd /d $(APP_ROOT_DIR)

    KCONFIG_BASIC_CMD :=$(KCONFIG_ROOT_DIR)/kconfig-mconf.exe $(MAIN_KCONFIG)
    KCONFIG_CMD :=$(KCONFIG_BASIC_CMD) 2>$(ERROR_LOG)

    MANUAL_KCONFIG_CMD :=$(CHNG_DIR_CMD) & $(SET_ENV_CMD) &
    MANUAL_KCONFIG_CMD +=$(KCONFIG_BASIC_CMD)

    MANUAL_KCONFIG_WARNING_CHECK_CMD :=$(CHNG_DIR_CMD) & $(SET_ENV_CMD) &
    MANUAL_KCONFIG_WARNING_CHECK_CMD +=$(KCONFIG_BASIC_CMD) 1>$(ERROR_LOG)

    NEW_WIN_KCONFIG_CMD :=$(SET_ENV_CMD) & start $(KCONFIG_CMD)

    KCONFIG_PRINT_ERRORS_CMD :=type $(ERROR_LOG)

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    ERROR_LOG =  $(OUT_DIR)/kconfig.out


##   $(info err: $(SHELL_OUTPUT))
##   $(info ---: you need to install kconfig frontends)
##   $(info ---: example of installing in ubuntu  :)
##   $(info ---: ----------------------------------)
##   $(info ---: sudo apt-get install gperf libncurses5-dev)
##   $(info ---: cd ~;git clone https://patacongo@bitbucket.org/nuttx/tools.git tools)
##   $(info ---: cd tools/kconfig-frontends/)
##   $(info ---: ./configure --enable-mconf --disable-shared --enable-static)
##   $(info ---: make)
##   $(info ---: sudo make install )
##   $(info ---: ----------------------------------)
##   $(info ---: after installing run kconfig-mconf in shell and check that you)
##   $(info ---: get "can't find file ..." output only)


	$(info err: TODO : add KCONFIG_START_DIR_PATH=$(KCONFIG_START_DIR_PATH) to shell environment)
	$(call exit,1)
    KCONFIG_CMD :=kconfig-mconf $(MAIN_KCONFIG) 2>$(ERROR_LOG)
    KCONFIG_CMD_FOR_WARNING_CHECK_CMD :=kconfig-mconf $(MAIN_KCONFIG) 1>$(ERROR_LOG)
    MANUAL_KCONFIG_CMD :=cd $(APP_ROOT_DIR) ; $(KCONFIG_CMD)
    MANUAL_KCONFIG_WARNING_CHECK_CMD :=cd $(APP_ROOT_DIR) ; $(KCONFIG_CMD_FOR_WARNING_CHECK)
    NEW_WIN_KCONFIG_CMD :=xterm -geometry 120x40 -fa DejaVuSansMono -fs 9 -e "$(KCONFIG_CMD)"
    KCONFIG_PRINT_ERRORS_CMD :=cat $(ERROR_LOG)

endif



$(info running : $(NEW_WIN_KCONFIG_CMD))
SHELL_OUTPUT :=$(shell $(NEW_WIN_KCONFIG_CMD))
ifneq ($(findstring Redirection is not supported,$(SHELL_OUTPUT)),)
    $(info err: you can run kconfig utility only from shell.)
    $(info ---: open shell and run the following :)
    $(info ---: $(ENTER_PROJECT_DIR) $(SHELL_CMD_DELIMITER) $(MAKE) menuconfig)
    $(call exit,1)
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
	$(info --- if kconfig window didn't opened, run following command in shell:)
	$(info --- $(MANUAL_KCONFIG_CMD) )
	$(info --- to see kconfig warnings run following command in shell : )
	$(info --- $(MANUAL_KCONFIG_WARNING_CHECK_CMD) )
	$(info auto generated Kconfig created)
