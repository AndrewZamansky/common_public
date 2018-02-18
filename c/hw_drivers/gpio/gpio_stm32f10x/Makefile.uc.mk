ifeq ($(sort $(CONFIG_STM32F103RC)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_GPIO)
endif



#INCLUDE_DIR =

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = gpio_stm32f10x.c
VPATH = src

SRC += stm32f10x_gpio.c
VPATH +=  | $(STM32F10X_SRC_DIR)

include $(COMMON_CC)
