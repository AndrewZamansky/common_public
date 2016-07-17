
ifdef CONFIG_POLEG
	INCLUDE_THIS_COMPONENT := y
endif

SRC = board_init.c

VPATH = src

include $(COMMON_CC)
