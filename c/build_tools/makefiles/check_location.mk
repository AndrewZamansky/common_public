include $(MAKEFILES_ROOT_DIR)/common.mk

ifneq ($(strip $(WORKSPACE_ROOT_DIR_FOR_TEST)),$(WORKSPACE_ROOT_DIR))
    $(error !---  workspace location changed . run 'make clean' .)
endif
