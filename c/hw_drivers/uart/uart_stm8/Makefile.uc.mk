ifeq ($(sort $(CONFIG_STM8S103F)),y)
    INCLUDE_THIS_COMPONENT :=$(CONFIG_INCLUDE_INTERNAL_UART)
endif


#INCLUDE_DIR =

#DEFINES =

#CFLAGS =

#ASMFLAGS =



SRC = uart_stm8.c

VPATH = src

SRC += stm8s_uart1.c

VPATH += | $(STM8S_SRC_DIR)

include $(COMMON_CC)
