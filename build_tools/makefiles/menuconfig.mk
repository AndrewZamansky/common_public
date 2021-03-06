
include $(MAKEFILES_ROOT_DIR)/common.mk
ENTER_PROJECT_DIR :=
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ENTER_PROJECT_DIR += $(COMMON_PARTITION) & 
endif
ENTER_PROJECT_DIR += cd $(APP_ROOT_DIR)


##   $(info err: $(SHELL_OUTPUT))
##   $(info ---: you need to install kconfig frontends)
##   $(info ---: example of installing in ubuntu  :)
##   $(info ---: ----------------------------------)
##   $(info ---: sudo apt-get install gperf libncurses5-dev)
##   $(info ---: cd ~;git clone https://patacongo@bitbucket.org/nuttx/tools.git tools)
##   $(info ---: cd tools/kconfig-frontends/)
##   $(info ---: ./configure --enable-mconf --disable-shared --enable-static)
##   $(info ---: make)
##   $(info ---: sudo make install                    )
##   $(info ---:                                      )
##   $(info ---: OR install binaries from PPA         )
##   $(info ---: sudo add-apt-repository ppa:george-hopkins/kconfig-frontends  )
##   $(info ---: sudo apt-get update                  )
##   $(info ---: sudo apt-get install kconfig-all     )
##   $(info ---:                                      )
##   $(info ---: ----------------------------------)
##   $(info ---: after installing run kconfig-mconf in shell and check that you)
##   $(info ---: get "can't find file ..." output only)




### test for existence of kconfig and put its directory name in GIT_ROOT_DIR ###
SEARCHED_TOOL:=kconfig-mconf
SEARCHED_DIR_VARIABLE:=KCONFIG_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_KCONFIG_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=kconfig-mconf.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=kconfig-mconf
endif
include $(MAKEFILES_INC_FUNC_DIR)/tool_existence_check.mk
####### end of tool existence test #####



# if no Kconfig in "common_priveate" then create dummy one
COMMON_PRIVATE_KCONFIG_FILE :=$(KCONFIG_PRIVATE_START_DIR)/Kconfig
ifeq ("$(wildcard $(COMMON_PRIVATE_KCONFIG_FILE))","")
    $(call mkdir_if_not_exists, $(dir $(COMMON_PRIVATE_KCONFIG_FILE)))
    DUMMY:=$(shell echo config PRIVATE_DUMMY>$(COMMON_PRIVATE_KCONFIG_FILE))
    DUMMY:=$(shell echo     bool "dummy">>$(COMMON_PRIVATE_KCONFIG_FILE))
endif

PROJECT_KCONFIG :=$(call fix_path_if_in_windows,$(PROJECT_KCONFIG))

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)

    ERROR_LOG =  $(OUT_DIR)\kconfig.out
    
    SHELL_GO_TO_GIT_DIR :=cd /d $(call fix_path_if_in_windows,$(APP_ROOT_DIR)) &

    KCONFIG_BASIC_CMD :=$(KCONFIG_ROOT_DIR)/kconfig-mconf.exe $(PROJECT_KCONFIG)
    KCONFIG_CMD :=$(KCONFIG_BASIC_CMD) 2>$(ERROR_LOG)

    MANUAL_KCONFIG_CMD +=$(KCONFIG_BASIC_CMD)

    MANUAL_KCONFIG_WARNING_CHECK_CMD +=$(KCONFIG_BASIC_CMD) 1>$(ERROR_LOG)

    NEW_WIN_KCONFIG_CMD :=start $(KCONFIG_CMD)

    KCONFIG_PRINT_ERRORS_CMD :=type $(ERROR_LOG)

else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)

    ERROR_LOG =  $(OUT_DIR)/kconfig.out

    SHELL_GO_TO_GIT_DIR :=cd $(APP_ROOT_DIR) ;

    KCONFIG_BASIC_CMD :=$(KCONFIG_ROOT_DIR)/kconfig-mconf $(PROJECT_KCONFIG)
    KCONFIG_CMD :=$(KCONFIG_BASIC_CMD) 2>$(ERROR_LOG)

    MANUAL_KCONFIG_CMD +=$(KCONFIG_BASIC_CMD)

    MANUAL_KCONFIG_WARNING_CHECK_CMD +=$(KCONFIG_BASIC_CMD) 1>$(ERROR_LOG)
    
    NEW_WIN_KCONFIG_CMD :=xterm -geometry 120x40 -fa DejaVuSansMono
    NEW_WIN_KCONFIG_CMD +=  -fs 9 -e "$(KCONFIG_CMD)"

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
	$(info --- $(SHELL_GO_TO_GIT_DIR) $(MANUAL_KCONFIG_CMD) )
	$(info --- to see kconfig warnings run following command in shell [note: graphical window will NOT open)
	$(info ---  and ignore "LINES value must be >= 2 and <= 0..." and "initscr(): Unable to create SP"] : )
	$(info --- $(SHELL_GO_TO_GIT_DIR) $(MANUAL_KCONFIG_WARNING_CHECK_CMD) )
	$(info auto generated Kconfig created)
