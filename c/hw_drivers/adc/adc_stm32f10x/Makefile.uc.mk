ifeq ($(sort $(CONFIG_STM32F10X)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_ADC)
endif

#INCLUDE_DIR +=

#DEFINES +=

#CFLAGS +=

#ASMFLAGS +=



SRC = adc_stm32f10x.c
VPATH = src

SRC += stm32f10x_adc.c
VPATH += | $(STM32F10X_DRIVERS_DIR)/src

include $(COMMON_CC)
