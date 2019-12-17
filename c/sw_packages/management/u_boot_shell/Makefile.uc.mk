ifeq ($(sort $(CONFIG_INCLUDE_UBOOT_SHELL)),y)
    INCLUDE_THIS_COMPONENT := y

    EXPANDED_MAKEFILE_LIST := $(realpath $(MAKEFILE_LIST))
    CURR_FILE_SUFFIX :=u_boot_shell/Makefile.uc.mk
    CURR_MAKEFILE :=$(filter %$(CURR_FILE_SUFFIX), $(EXPANDED_MAKEFILE_LIST))
    CURR_MAKEFILE :=$(sort $(CURR_MAKEFILE)) # remove dublicates
    CURR_COMPONENT_DIR := $(patsubst %/Makefile.uc.mk,%,$(CURR_MAKEFILE))

    U_BOOT_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/u-boot
    ifeq ("$(wildcard $(U_BOOT_PATH))","")
        $(info   )
        $(info !--- u-boot path $(U_BOOT_PATH) dont exists )
        $(info !--- get u-boot repository from git://git.denx.de/u-boot.git $(U_BOOT_PATH))
        $(info !--- make sure that .git directory is located in $(U_BOOT_PATH)/  after unpacking)
        $(error )
    endif

    # test if current commit and branch of uboot git is the same
    # as required by application
    CURR_GIT_REPO_DIR :=$(U_BOOT_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_UBOOT_GIT_COMMIT_HASH
    CURR_GIT_BUNDLE :=$(CURR_COMPONENT_DIR)/u-boot.bundle
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

endif

DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,$(CURR_COMPONENT_DIR)/include )
DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,$(EXTERNAL_SOURCE_ROOT_DIR))

DEFINES =
CFLAGS =

ifdef CONFIG_GPP
    CFLAGS = -fpermissive
endif

ifdef CONFIG_MICROSOFT_COMPILER
    CFLAGS += /wd4189
    CFLAGS += /wd4127
    CFLAGS += /wd4200
    
    # to disable deprecation in windows compiler :
	DEFINES += _CRT_SECURE_NO_WARNINGS
endif

#ASMFLAGS =

INCLUDE_DIR = $(U_BOOT_PATH)/include#  $(U_BOOT_PATH)/arch/arm/include/asm/

ifneq ($(sort $(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)),y)
    SRC = u_boot_shell.c
endif

ifdef CONFIG_UBOOT_CMD_GET_SERIAL
    SRC += cmd_get_serial.c
endif
ifdef CONFIG_UBOOT_CMD_GET_VERSION
    SRC += cmd_get_version.c
endif
ifdef CONFIG_UBOOT_CMD_HEAP_TEST
    SRC += cmd_heap_test.c
endif
ifdef CONFIG_UBOOT_CMD_BOARD_IP
    SRC += cmd_board_ip.c
endif
ifdef CONFIG_UBOOT_CMD_BOARD_NAME
    SRC += cmd_board_name.c
endif
ifdef CONFIG_UBOOT_CMD_GET_DEV_PARAM
    SRC += cmd_get_dev_param.c
endif
ifdef CONFIG_UBOOT_CMD_SET_DEV_PARAM
    SRC += cmd_set_dev_param.c
endif
ifdef CONFIG_UBOOT_CMD_PRINTF_DEV
    SRC += cmd_printf_dev.c
endif
ifdef CONFIG_UBOOT_CMD_SPI_FORMAT
    SRC += cmd_spi_format.c
endif

VPATH = src

SRC += command.c
SRC += cli.c
SRC += cli_simple.c
SRC += cmd_help.c


VPATH += | $(U_BOOT_PATH)/common


ifdef CONFIG_INCLUDE_ONLY_UBOOT_SHELL_API
    DEFINES += UBOOT_SHELL_PROJ_NAME=$(CONFIG_PROJECT_NAME)
    #only compile following file:
    SRC := uboot_shell_api_check.c
endif

include $(COMMON_CC)
