
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_DSP)


#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = dsp_management.cpp
VPATH = src

ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FIXED_POINT
    ifdef CONFIG_FIXMATH_NO_CACHE
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES, FIXMATH_NO_CACHE )
    endif
    ifdef CONFIG_FIXMATH_NO_OVERFLOW
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES, FIXMATH_NO_OVERFLOW )
    endif
    ifdef CONFIG_FIXMATH_NO_ROUNDING
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES, FIXMATH_NO_ROUNDING )
    endif
    LIBFIXMATH_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/libfixmath/trunk/libfixmath
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH, $(LIBFIXMATH_PATH))
    SRC += fix16.c
    SRC += fix16_exp.c
    SRC += fix16_sqrt.c
    SRC += fix16_str.c
    SRC += fix16_trig.c
    SRC += fract32.c
    SRC += uint32.c
    VPATH += | $(LIBFIXMATH_PATH)
endif

ifeq ($(CONFIG_XTENSA_FUSIONF1_P7_G60_NTCA),y)
    NATURE_DSP_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/NatureDSP_FusionF1
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH, $(NATURE_DSP_PATH)/include)
    DUMMY := $(call \
                ADD_TO_GLOBAL_INCLUDE_PATH, $(NATURE_DSP_PATH)/include_private)
endif

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += dsp_management.cpp
endif



include $(COMMON_CC)
