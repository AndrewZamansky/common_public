
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_DELAY)


#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  




SRC = delay.cpp

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += delay.cpp
endif

VPATH = src


include $(COMMON_CC)
