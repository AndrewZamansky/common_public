
ifeq ($(sort $(CONFIG_I96XXX_M0)),y)

    BSP_I96XXX_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/BSP_I96XXX
    ifeq ("$(wildcard $(BSP_I96XXX_PATH))","")
        $(info !--- I96XXX BSP path $(BSP_I96XXX_PATH) does not exist)
        $(info !--- get I96XXX BSP git and unpack it to $(BSP_I96XXX_PATH))
        $(info !--- make sure Library and SampleCode folders are)
        $(info !--- located in $(BSP_I96XXX_PATH)/  after unpacking)
        $(error )
    endif

    # test if current commit and branch of uboot git is the same
    # as required by application
    CURR_GIT_REPO_DIR :=$(BSP_I96XXX_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_BSP_I96XXX_GIT_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    ifeq ("","$(filter $(BSP_I96XXX_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(BSP_I96XXX_PATH)
    endif

    INC_PATH := $(BSP_I96XXX_PATH)/Library/StdDriver/inc
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(INC_PATH))
    INC_PATH := $(BSP_I96XXX_PATH)/Library/Device/Nuvoton/I96100/Include
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(INC_PATH))
    INC_PATH := $(BSP_I96XXX_PATH)/Library/Device/Nuvoton/Include
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(INC_PATH))
    I96XXX_SRC_DIR := $(BSP_I96XXX_PATH)/Library/StdDriver/src
    INCLUDE_THIS_COMPONENT := y

endif

SRC += board_init.c
VPATH = src

SRC += sys.c
VPATH +=  | $(I96XXX_SRC_DIR)

include $(COMMON_CC)
