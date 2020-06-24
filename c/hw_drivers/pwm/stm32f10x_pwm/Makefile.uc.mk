
INCLUDE_THIS_COMPONENT :=$(CONFIG_INCLUDE_INTERNAL_PWM)

ifeq ($(findstring stm32f10x,$(CONFIG_SOC_TYPE)),stm32f10x)
    INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc
endif

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = pwm_stm32f10x.c 
VPATH = src

SRC += stm32f10x_tim.c
VPATH += | $(STM32F10X_DRIVERS_DIR)/src

include $(COMMON_CC)
