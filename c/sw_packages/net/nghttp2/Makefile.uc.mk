
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_NGHTTP2)


ifdef CONFIG_INCLUDE_NGHTTP2
    EXPANDED_MAKEFILE_LIST := $(realpath $(MAKEFILE_LIST))
    CURR_FILE_SUFFIX :=nghttp2/Makefile.uc.mk
    CURR_MAKEFILE :=$(filter %$(CURR_FILE_SUFFIX), $(EXPANDED_MAKEFILE_LIST))
    CURR_COMPONENT_DIR := $(patsubst %/Makefile.uc.mk,%,$(CURR_MAKEFILE))

    # test if current commit and branch of nghttp2 git is the same
    # as required by application
    CURR_GIT_REPO_DIR :=$(CURR_COMPONENT_DIR)/nghttp2_git
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_NGHTTP2_GIT_MANAGER_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    include $(CURR_COMPONENT_DIR)/nghttp2_git/Makefile.submodule.uc.mk

endif
