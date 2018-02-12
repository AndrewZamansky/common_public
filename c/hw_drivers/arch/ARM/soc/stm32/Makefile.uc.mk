
ifeq ($(sort $(CONFIG_STM32F103RC)),y)

	BSP_STM32F10X_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver
    ifeq ("$(wildcard $(BSP_STM32F10X_PATH))","")
        $(info !--- STM32F10X BSP path $(BSP_STM32F10X_PATH) dont exists )
        $(info !--- download STM32F10X BSP (tested version 3.6.1) and unpack it to $(BSP_STM32F10X_PATH)  )
        $(info !--- make sure inc and src  folders is located in $(BSP_STM32F10X_PATH)/  after unpacking   )
        $(error )
    endif
    
    ifeq ("","$(filter $(BSP_STM32F10X_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(BSP_STM32F10X_PATH)
    endif
    
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(BSP_STM32F10X_PATH)/inc )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,STM32F10X_MD )
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,USE_STM3210B_EVAL )
	STM32F10X_SRC_DIR := $(BSP_STM32F10X_PATH)/src
	INCLUDE_THIS_COMPONENT := y

endif

SRC = board_init.c

VPATH = src

include $(COMMON_CC)
