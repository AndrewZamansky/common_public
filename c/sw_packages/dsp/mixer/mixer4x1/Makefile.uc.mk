
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_4X1_MIXER)
  

#DEFINES =

#CFLAGS =

#ASMFLAGS =  

SRC = mixer4x1.cpp

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += mixer4x1.cpp
endif


VPATH = src


include $(COMMON_CC)
