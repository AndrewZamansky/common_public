
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_WOLFSSL)

ifeq ($(strip $(CONFIG_INCLUDE_WOLFSSL)),y)

    CURR_COMPONENT_DIR :=\
       $(patsubst %/,%,$(dir $(realpath $(lastword $(MAKEFILE_LIST)))))

    # test if current commit and branch of nghttp2 git is
    # the same as required by application
    CURR_GIT_REPO_DIR :=$(CURR_COMPONENT_DIR)/wolfssl_git
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_WOLFSSL_GIT_MANAGER_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    include $(CURR_COMPONENT_DIR)/wolfssl_git/Makefile.submodule.uc.mk

    KCONFIG_DIRS += WOLFSSL_GIT_DIR=$(CURR_COMPONENT_DIR)/wolfssl_git
else
    KCONFIG_DIRS += WOLFSSL_GIT_DIR=$(KCONFIG_DUMMY)
endif
