INCLUDE_THIS_COMPONENT :=$(CONFIG_GENERIC_ARM_SOC)

SRC += board_init.c
VPATH = src

IS_LIBRARY :=$(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)

ifdef COMMON_CC#  COMMON_CC is defined during compilation stage
IS_LIBRARY :=$(filter y,$(IS_LIBRARY))
ifeq ($(IS_LIBRARY),y)
all:
	@echo no need to compile board related code in library
else
    include $(COMMON_CC)
endif
endif #for ifdef COMMON_CC
