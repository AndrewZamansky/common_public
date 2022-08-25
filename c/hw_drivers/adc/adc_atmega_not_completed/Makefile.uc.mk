ifeq ($(sort $(CONFIG_NUC505)),y)
    INCLUDE_THIS_COMPONENT = $(CONFIG_INCLUDE_INTERNAL_ADC)
endif


INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = adc_atmega.c
VPATH = src

SRC += stm32f10x_adc.c 
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/src

include $(COMMON_CC)
