
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_UBOOT_SHELL)

ifdef CONFIG_INCLUDE_UBOOT_SHELL

    CURR_UBOOT_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %u_boot_shell/Makefile.uc.mk,$(MAKEFILE_LIST))))
    U_BOOT_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/u-boot
    ifeq ("$(wildcard $(U_BOOT_PATH))","")
        $(info   )
        $(info !--- u-boot path $(U_BOOT_PATH) dont exists )
        $(info !--- get u-boot repository from git://git.denx.de/u-boot.git $(U_BOOT_PATH)  )
        $(info !--- make sure that .git directory is located in $(U_BOOT_PATH)/  after unpacking   )
        $(error )
    endif

    # test if current commit and branch of uboot git is the same
    # as required by application
    CURR_GIT_REPOSITORY_DIR :=$(U_BOOT_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_UBOOT_GIT_COMMIT_HASH
    CURR_GIT_BUNDLE :=$(CURR_UBOOT_COMPONENT_LOCATION)/u-boot.bundle
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(CURR_UBOOT_COMPONENT_LOCATION)/include )
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/)
endif

DEFINES =
CFLAGS =

ifdef CONFIG_GPP
    CFLAGS = -fpermissive
endif

ifdef CONFIG_MICROSOFT_COMPILER
    CFLAGS += /wd4189
    CFLAGS += /wd4127
    CFLAGS += /wd4200
	DEFINES += _CRT_SECURE_NO_WARNINGS # to disable deprecation in windows compiler
endif

#ASMFLAGS =

INCLUDE_DIR = $(U_BOOT_PATH)/include#  $(U_BOOT_PATH)/arch/arm/include/asm/

SRC = u_boot_shell.c

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

include $(COMMON_CC)
