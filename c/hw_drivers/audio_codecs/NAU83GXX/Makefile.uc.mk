ifeq ($(sort $(CONFIG_INCLUDE_NAU83GXX)),y)
    INCLUDE_THIS_COMPONENT := y
endif

#DEFINES =

#CFLAGS =

#ASMFLAGS =

# !!IMPORTANT!! : should be used before any 'include' statement in makefile:
CURR_COMPONENT_DIR :=$(call get_curr_component_dir)

SRC += src/NAU83GXX.c
SRC += src/NAU83GXX_utils.c
SRC += src/nau83gxx_routine_tasks.c

ADDITIONAL_DEPS += src/NAU83GXX2_kcs_protocol.c
ADDITIONAL_DEPS += src/NAU83GXX_G10.c
ADDITIONAL_DEPS += src/NAU83GXX_G20.c
ADDITIONAL_DEPS += src/NAU83GXX_G60.c


ifeq ($(sort $(CONFIG_NAU83GXX_ENABLE_DB_LAB_SHELL_COMMANDS)),y)
    SRC += src/shell_commands/cmd_kcs_init_nau83gxx_chip.c
    SRC += src/shell_commands/cmd_kcs_get_status.c
    SRC += src/shell_commands/cmd_kcs_get_counter.c
    SRC += src/shell_commands/cmd_kcs_get_data.c
    SRC += src/shell_commands/cmd_kcs_set_start.c
    SRC += src/shell_commands/cmd_kcs_set_add_data.c
    SRC += src/shell_commands/cmd_kcs_set_send.c
    SRC += src/shell_commands/cmd_kcs_swrev.c
    SRC += src/shell_commands/cmd_kcs_clkstart.c
    SRC += src/shell_commands/cmd_kcs_clkstop.c
    SRC += src/shell_commands/cmd_kcs_get_setup.c
    SRC += src/shell_commands/cmd_kcs_set_sw_alc.c

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
    SRC += src/shell_commands/cmd_kcs_read_i2c_reg.c
    SRC += src/shell_commands/cmd_kcs_write_i2c_reg.c
endif
SRC += src/shell_commands/cmd_kcs_send_setup_bin.c

VPATH =


ifeq ($(sort $(CONFIG_NAU83GXX_USE_KCS_REMOTE_INTERFACE)),y)
    KCS_REMOTE_INTERFACE_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/kcs_remote_interface
    ifeq ("$(wildcard $(KCS_REMOTE_INTERFACE_PATH))","")
        $(info !--- KCS remote interface path $(KCS_REMOTE_INTERFACE_PATH) does not exist)
        $(info !--- get KCS remote interface git and unpack it to $(KCS_REMOTE_INTERFACE_PATH))
        $(info !--- make sure .git directory is)
        $(info !--- located in $(KCS_REMOTE_INTERFACE_PATH)/  after unpacking)
        $(error )
    endif

    # test if current commit and branch of uboot git is the same
    # as required by application
    CURR_GIT_REPO_DIR :=$(KCS_REMOTE_INTERFACE_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=\
                    CONFIG_NAU83GXX_USE_KCS_REMOTE_INTERFACE_GIT_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    INCLUDE_DIR += $(KCS_REMOTE_INTERFACE_PATH)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH,\
                      $(KCS_REMOTE_INTERFACE_PATH)/include)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH, $(CURR_COMPONENT_DIR)/include)
    CFLAGS += -include kcs_port.h

    SRC += src/kcs_remote_interface_wrapper.c
    SRC += src/CTRCoreModules/KCS_remote_interface.c 
    SRC += src/shell_commands/cmd_kcs_get_info.c

    VPATH += | $(KCS_REMOTE_INTERFACE_PATH)
endif


include $(COMMON_CC)
