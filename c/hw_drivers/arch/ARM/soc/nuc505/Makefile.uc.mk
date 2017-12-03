
ifdef CONFIG_NUC505

    NUC505_BSP_VERSION :=$(patsubst "%",%,$(CONFIG_NUC505_BSP_VERSION))
    BSP_NUC505_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v$(NUC505_BSP_VERSION)
    ifeq ("$(wildcard $(BSP_NUC505_PATH))","")
        $(info !--- NUC505 BSP path $(BSP_NUC505_PATH) dont exists )
        $(info !--- download NUC505 BSP version $(NUC505_BSP_VERSION) and unpack it to $(BSP_NUC505_PATH)  )
        $(info !--- make sure Library and SampleCode  folders is located in $(BSP_NUC505_PATH)/  after unpacking   )
        $(error )
    endif
    
    ifeq ("","$(filter $(BSP_NUC505_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(BSP_NUC505_PATH)
    endif
    
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(BSP_NUC505_PATH)/Library/StdDriver/inc )
	NUC505_SRC_DIR := $(BSP_NUC505_PATH)/Library/StdDriver/src
	INCLUDE_THIS_COMPONENT := y
	
endif

SRC = board_init.c
VPATH = src

SRC += sys.c
VPATH +=  | $(NUC505_SRC_DIR)

include $(COMMON_CC)
