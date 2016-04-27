
ifdef CONFIG_NUC505

    ifeq ("$(wildcard $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003)","")
        $(info !!!   $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003 don't exists !!!! )
        $(error )
    endif	
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/Device/Nuvoton/NUC505Series/Include )
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/StdDriver/inc )
	NUC505_SRC_DIR := $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/StdDriver/src
	INCLUDE_THIS_COMPONENT := y
endif

SRC = board_init.c

VPATH = src

include $(COMMON_CC)
