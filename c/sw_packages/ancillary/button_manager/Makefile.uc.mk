ifeq ($(call eval_config,$(CONFIG_INCLUDE_BUTTON_MANAGER)),y)
    INCLUDE_THIS_COMPONENT := y
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = button_manager.c

VPATH = src

include $(COMMON_CC)
