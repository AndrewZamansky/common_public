
ifeq ($(sort $(CONFIG_I94XXX)),y)

    I94XXX_BSP_VERSION :=$(patsubst "%",%,$(CONFIG_I94XXX_BSP_VERSION))
    _BSP_DIR_NAME =BSP_I94XXX_v$(I94XXX_BSP_VERSION)
    BSP_I94XXX_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/$(_BSP_DIR_NAME)
    ifeq ("$(wildcard $(BSP_I94XXX_PATH))","")
        $(info !--- I94XXX BSP path $(BSP_I94XXX_PATH) does not exist)
        $(info !--- download I94XXX BSP version $(I94XXX_BSP_VERSION) and)
        $(info !--- unpack it to $(BSP_I94XXX_PATH).  Make sure Library and)
        $(info !--- SampleCode folders is located)
        $(info !--- in $(BSP_I94XXX_PATH)/  after unpacking)
        $(error )
    endif

    ifeq ("","$(filter $(BSP_I94XXX_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(BSP_I94XXX_PATH)
    endif

    INC_PATH := $(BSP_I94XXX_PATH)/Library/StdDriver/inc
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(INC_PATH))
    INC_PATH := $(BSP_I94XXX_PATH)/Library/Device/Nuvoton/I94100/Include
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(INC_PATH))
    INC_PATH := $(BSP_I94XXX_PATH)/Library/Device/Nuvoton/Include
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(INC_PATH))
	I94XXX_SRC_DIR := $(BSP_I94XXX_PATH)/Library/StdDriver/src
	INCLUDE_THIS_COMPONENT := y
	
endif

SRC += board_init.c
VPATH = src

SRC += sys.c
VPATH +=  | $(I94XXX_SRC_DIR)

include $(COMMON_CC)
