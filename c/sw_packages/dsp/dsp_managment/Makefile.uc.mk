
ifeq ($(findstring YES,$(CONFIG_INCLUDE_DSP_MANAGMENT)),YES) 	 
    _INCLUDE_COMMON_DSP :=YES	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  




SRC = dsp_managment.c

ifeq ($(findstring AS_SPEED_CRITICAL,$(CONFIG_INCLUDE_DSP_MANAGMENT)),AS_SPEED_CRITICAL) 
    SPEED_CRITICAL_FILES += dsp_managment.c
    _COMMON_DSP_IS_SPEED_CRITICAL := YES
endif

VPATH = src


include $(COMMON_CC)
