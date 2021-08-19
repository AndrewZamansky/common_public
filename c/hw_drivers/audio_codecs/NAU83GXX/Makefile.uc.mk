ifeq ($(sort $(CONFIG_INCLUDE_NAU83GXX)),y)
    INCLUDE_THIS_COMPONENT := y
endif

#DEFINES =

#CFLAGS =

#ASMFLAGS =

SRC += src/NAU83GXX.c
SRC += src/NAU83GXX_utils.c


ifeq ($(sort $(CONFIG_NAU83GXX_ENABLE_SHELL_COMMANDS)),y)
    SRC += src/shell_commands/cmd_init_83g10_dsp.c
    SRC += src/shell_commands/cmd_dsp_counter.c
    SRC += src/shell_commands/cmd_dsp_status.c
    SRC += src/shell_commands/cmd_dsp_clkstop.c
    SRC += src/shell_commands/cmd_dsp_clkstart.c
    SRC += src/shell_commands/cmd_dsp_swrev.c
    SRC += src/shell_commands/cmd_dsp_getkcs_setup.c
    SRC += src/shell_commands/cmd_dsp_getkcs.c
    SRC += src/shell_commands/cmd_dsp_setkcs_start.c
    SRC += src/shell_commands/cmd_dsp_setkcs_store.c
    SRC += src/shell_commands/cmd_dsp_setkcs_send.c
    SRC += src/shell_commands/cmd_kcs_send_setup_bin.c
endif

VPATH =

include $(COMMON_CC)
