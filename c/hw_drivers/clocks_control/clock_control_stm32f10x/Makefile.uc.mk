ifeq ($(sort $(CONFIG_STM32F103RC)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)
endif

#INCLUDE_DIR =

#DEFINES = 

#CFLAGS =

#ASMFLAGS =



SRC = clocks_control_stm32f10x.c
VPATH = src

SRC += stm32f10x_rcc.c
VPATH +=   | $(STM32F10X_SRC_DIR)

include $(COMMON_CC)
