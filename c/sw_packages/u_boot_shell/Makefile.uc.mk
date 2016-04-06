
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_UBOOT_SHELL)

ifdef CONFIG_INCLUDE_UBOOT_SHELL
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH ,  $(SW_PACKAGES_ROOT_DIR)/u_boot_shell/include )
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/include)
endif
#DEFINES = 

ifdef CONFIG_GPP
    CFLAGS = -fpermissive
endif

#ASMFLAGS =  



SRC = uboot.c

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


VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/u-boot/common

include $(COMMON_CC)
