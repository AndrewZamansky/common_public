ifeq ($(sort $(CONFIG_INCLUDE_NAU88L24)),y)
    INCLUDE_THIS_COMPONENT := y
endif


#DEFINES =

#CFLAGS =

#ASMFLAGS =



SRC += NAU88L24.c

#SPEED_CRITICAL_FILES +=

VPATH = src

include $(COMMON_CC)
