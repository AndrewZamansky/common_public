ifeq ($(sort $(CONFIG_STM8S103F)),y)
    INCLUDE_THIS_COMPONENT :=$(CONFIG_INCLUDE_INTERNAL_TIMER)
endif


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = stm8_tim1.c 
VPATH = src

SRC += stm8s_tim1.c

VPATH += | $(STM8S_SRC_DIR)

include $(COMMON_CC)
