
ifdef CONFIG_NPCE586

	INCLUDE_THIS_COMPONENT := y
	
endif

SRC = board_init.c

VPATH = src

include $(COMMON_CC)
