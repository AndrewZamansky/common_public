
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_COMMON_DSP) $(CONFIG_AUTO_INCLUDE_COMMON_DSP)

#INCLUDE_DIR =

#DEFINES =

#CFLAGS =

#ASMFLAGS =




SRC  = math_functions.cpp
ifdef CONFIG_DSP_REAL_NUMBER_FORMAT_FIXED_POINT
    SRC += math_functions_fixed_point.cpp
endif
VPATH = src


ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += math_functions.cpp
    SPEED_CRITICAL_FILES += math_functions_fixed_point.cpp
endif


include $(COMMON_CC)
