
ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_DMIC)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = dmic_i94xxx.c

VPATH = src

SRC += dmic.c

VPATH += | $(I94XXX_SRC_DIR)

include $(COMMON_CC)
