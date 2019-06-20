ifeq ($(sort $(CONFIG_STM32F10X)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)
endif

#INCLUDE_DIR =

#DEFINES = 

#CFLAGS =

#ASMFLAGS =



SRC = clocks_control_stm32f10x.c
VPATH = src


include $(COMMON_CC)
