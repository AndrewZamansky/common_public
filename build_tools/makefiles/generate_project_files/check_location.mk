include $(MAKEFILES_ROOT_DIR)/common.mk

ifneq ($(strip $(WORKSPACE_ROOT_DIR_FOR_TEST)),$(PARENT_OF_COMMON_PUBLIC_DIR))
    $(info err:  workspace location changed . run 'make clean' .)
    $(call exit,1)
endif
