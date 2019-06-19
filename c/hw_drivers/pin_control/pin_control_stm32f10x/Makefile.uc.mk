ifeq ($(sort $(CONFIG_STM32F103XXX)),y)
    INCLUDE_THIS_COMPONENT := y
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = pin_control_stm32f10x.c
VPATH = src

SRC += stm32f10x_rcc.c
SRC += stm32f10x_gpio.c
VPATH +=  | $(STM32F10X_SRC_DIR)


include $(COMMON_CC)
