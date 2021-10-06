
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_NGHTTP2)


ifdef CONFIG_INCLUDE_NGHTTP2
    # !!IMPORTANT!! : should be used before any 'include' statement in makefile:
    CURR_COMPONENT_DIR :=$(call get_curr_component_dir)

    # test if current commit and branch of nghttp2 git is the same
    # as required by application
    CURR_GIT_REPO_DIR :=$(CURR_COMPONENT_DIR)/nghttp2_git
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_NGHTTP2_GIT_MANAGER_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    include $(CURR_COMPONENT_DIR)/nghttp2_git/Makefile.submodule.uc.mk

    KCONFIG_DIRS += NGHTTP2_GIT_DIR=$(CURR_COMPONENT_DIR)/nghttp2_git
else
    KCONFIG_DIRS += NGHTTP2_GIT_DIR=$(KCONFIG_DUMMY)
endif
