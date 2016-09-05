
ifdef CONFIG_STM8

	BSP_STM8S_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/ST/stm8s-periphs
    ifeq ("$(wildcard $(BSP_STM8S_PATH))","")
        $(info STM8 BSP path $(BSP_STM8S_PATH) dont exists )
        $(info download STM8 BSP (stm8s-periphs) and unpack it to $(BSP_STM8S_PATH)  )
        $(info make sure inc and src  folders is located in $(BSP_STM8S_PATH)/  after unpacking   )
        $(error )
    endif
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(BSP_STM8S_PATH)/inc )
	DUMMY := $(call ADD_TO_GLOBAL_DEFINES , STM8S103)
	STM8S_SRC_DIR := $(BSP_STM8S_PATH)/src
	INCLUDE_THIS_COMPONENT := y
	
endif

#SRC = board_init.c

VPATH = src

include $(COMMON_CC)
