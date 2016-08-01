
ifdef CONFIG_NUC505

	BSP_NUC505_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003
    ifeq ("$(wildcard $(BSP_NUC505_PATH))","")
        $(info !--- NUC505 BSP path $(BSP_NUC505_PATH) dont exists )
        $(info !--- download NUC505 BSP version 3.00.003 and unpack it to $(BSP_NUC505_PATH)  )
        $(info !--- make sure Library and SampleCode  folders is located in $(BSP_NUC505_PATH)/  after unpacking   )
        $(error )
    endif
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(BSP_NUC505_PATH)/Library/StdDriver/inc )
	NUC505_SRC_DIR := $(BSP_NUC505_PATH)/Library/StdDriver/src
	INCLUDE_THIS_COMPONENT := y
	
endif

SRC = board_init.c

VPATH = src

include $(COMMON_CC)
