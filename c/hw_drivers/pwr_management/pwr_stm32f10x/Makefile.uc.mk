ifeq ($(sort $(CONFIG_STM32F10X)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_POWER_MANAGEMENT)
endif

#INCLUDE_DIR +=

#DEFINES +=

#CFLAGS +=

#ASMFLAGS +=

SRC = pwr_stm32f10x.c
VPATH = src

SRC += stm32f10x_pwr.c
VPATH +=  | $(STM32F10X_SRC_DIR)

include $(COMMON_CC)