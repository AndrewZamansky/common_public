
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_2X2_MIXER)
  

#DEFINES =

#CFLAGS =

#ASMFLAGS =  

SRC = mixer2x2.cpp

ifdef CONFIG_DSP_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += mixer2x2.cpp
endif


VPATH = src


include $(COMMON_CC)
