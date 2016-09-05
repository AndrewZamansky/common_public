
ifdef CONFIG_NUC505
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_I2S)
endif



#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC = I2S_nuc505.c


ifdef CONFIG_INTERNAL_I2S_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += I2S_nuc505.c
endif


VPATH = src

SRC += i2s.c
VPATH += | $(NUC505_SRC_DIR)

include $(COMMON_CC)
