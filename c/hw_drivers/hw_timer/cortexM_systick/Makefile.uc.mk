
INCLUDE_THIS_COMPONENT :=$(CONFIG_INCLUDE_CORTEXM_SYSTICK)

ifeq ($(findstring stm32f10x,$(CONFIG_SOC_TYPE)),stm32f10x)
    INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc
endif

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = cortexM_systick.c 
VPATH = src


include $(COMMON_CC)
