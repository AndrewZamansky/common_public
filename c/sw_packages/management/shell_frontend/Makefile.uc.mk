
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_SHELL)

#INCLUDE_DIR =

#DEFINES =

#CFLAGS =

#ASMFLAGS =



SRC += shell_frontend.c
SRC += cmd_bin_0x00.c


VPATH = src

include $(COMMON_CC)
