
ifeq ($(sort $(CONFIG_ARBEL)),y)
	INCLUDE_THIS_COMPONENT := y
endif

SRC += board_init.c
VPATH = src


include $(COMMON_CC)
