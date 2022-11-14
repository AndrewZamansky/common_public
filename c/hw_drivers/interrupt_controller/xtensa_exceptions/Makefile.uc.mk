
ifeq (y,$(CONFIG_OUTPUT_TYPE_APPLICATION))
    INCLUDE_THIS_COMPONENT := $(CONFIG_XTENSA_XCC)
endif

#INCLUDE_DIR +=

#DEFINES +=

#CFLAGS +=

#ASMFLAGS +=

SRC += xtensa_intr.c

SRC += xtensa_intr_asm.S
ifeq (y,$(CONFIG_FREE_RTOS))
    SRC += xtensa_vectors.S
endif

VPATH += | src_xcc

ifdef CONFIG_CPU_SPECIFIC_FILES_ARE_SPEAD_CRITICAL 
    #SPEED_CRITICAL_FILES := $(SPEED_CRITICAL_FILES) $(SRC)
endif

include $(COMMON_CC)
