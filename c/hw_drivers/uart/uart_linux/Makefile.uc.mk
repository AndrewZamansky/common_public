
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_LINUX_UART)

ifeq ($(strip $(CONFIG_INCLUDE_LINUX_UART)),y)
    DUMMY := $(call ADD_TO_GLOBAL_LIBS , libpthread )
endif

#INCLUDE_DIR +=

#DEFINES +=

#CFLAGS +=

#ASMFLAGS +=



SRC = uart_linux.c

VPATH = src


include $(COMMON_CC)
