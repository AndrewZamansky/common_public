
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_TONE_GENERATOR)


#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  




SRC = tone_generator.cpp

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += tone_generator.cpp
endif

VPATH = src

ifneq ($(strip $(INCLUDE_THIS_COMPONENT)),)
    ifneq ($(and $(CONFIG_CORTEX_M4),$(CONFIG_INCLUDE_CORTEX_M_FPU)),)
        GLOBAL_LIBS += libarm_cortexM4lf_math.a
    endif
endif

include $(COMMON_CC)
