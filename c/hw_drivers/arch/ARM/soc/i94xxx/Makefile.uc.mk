
ifdef CONFIG_I94XXX

    I94XXX_BSP_VERSION :=$(patsubst "%",%,$(CONFIG_I94XXX_BSP_VERSION))
    BSP_I94XXX_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/BSP_I94XXX_v$(I94XXX_BSP_VERSION)
    ifeq ("$(wildcard $(BSP_I94XXX_PATH))","")
        $(info !--- I94XXX BSP path $(BSP_I94XXX_PATH) dont exists )
        $(info !--- download I94XXX BSP version $(I94XXX_BSP_VERSION) and unpack it to $(BSP_I94XXX_PATH)  )
        $(info !--- make sure Library and SampleCode  folders is located in $(BSP_I94XXX_PATH)/  after unpacking   )
        $(error )
    endif
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(BSP_I94XXX_PATH)/Library/StdDriver/inc )
	I94XXX_SRC_DIR := $(BSP_I94XXX_PATH)/Library/StdDriver/src
	INCLUDE_THIS_COMPONENT := y
	
endif

SRC = board_init.c
VPATH = src

SRC += sys.c
VPATH +=  | $(I94XXX_SRC_DIR)

include $(COMMON_CC)
