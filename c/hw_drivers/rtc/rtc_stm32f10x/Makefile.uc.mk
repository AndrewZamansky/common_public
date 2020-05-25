ifeq ($(sort $(CONFIG_STM32F10X)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_RTC)
endif

#INCLUDE_DIR +=

#DEFINES +=

#CFLAGS +=

#ASMFLAGS +=

SRC = rtc_stm32f10x.c
VPATH = src

SRC += stm32f10x_rtc.c
VPATH +=  | $(STM32F10X_SRC_DIR)

include $(COMMON_CC)
