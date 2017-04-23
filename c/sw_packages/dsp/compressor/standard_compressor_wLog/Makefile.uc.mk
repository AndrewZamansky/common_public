
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_STANDARD_COMPRESSOR_WITH_LOG)

ifneq ($(strip $(INCLUDE_THIS_COMPONENT)),)
    ifneq ($(and $(CONFIG_CORTEX_M4),$(CONFIG_INCLUDE_CORTEX_M_FPU)),)
        GLOBAL_LIBS += libarm_cortexM4lf_math.a
        GLOBAL_LIBS_PATH += $(ARM_CMSIS_PATH)/CMSIS/Lib/GCC
    endif
endif

#INCLUDE_DIR = 

#DEFINES =

#CFLAGS =

#ASMFLAGS =


SRC += standard_compressor_wLog.c
ifdef CONFIG_COMPRESSOR_IS_SPEED_CRITICAL
	SPEED_CRITICAL_FILES += standard_compressor_wLog.c
endif


VPATH = src


include $(COMMON_CC)
