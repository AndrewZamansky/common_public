
INCLUDE_THIS_COMPONENT := y


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = auto_init.c 

ifdef CONFIG_MICROSOFT_COMPILER
    SRC += auto_init_msvc_helper.c
else ifdef CONFIG_HEXAGON_COMPILER
    SRC += auto_init_gcc_helper.c
else ifdef CONFIG_ANDROID_NDK
    SRC += auto_init_gcc_helper.c
endif

VPATH = src

include $(COMMON_CC)
