ifeq ($(sort $(CONFIG_STM8S103F)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_SERIAL_NUMBER)
endif


INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/stm8s-periphs/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = serial_number_stm8.c 

ifdef CONFIG_INCLUDE_UBOOT_SHELL
    SRC += cmd_get_serial.c
endif

VPATH = src


include $(COMMON_CC)
