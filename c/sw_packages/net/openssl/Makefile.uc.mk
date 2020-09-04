
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_OPENSSL)

ifeq ($(strip $(CONFIG_INCLUDE_OPENSSL)),y)
    # calculation of CURR_COMPONENT_DIR must be before all 'include' directives
    CURR_COMPONENT_DIR :=\
       $(patsubst %/,%,$(dir $(realpath $(lastword $(MAKEFILE_LIST)))))

    # test if current commit and branch of nghttp2 git is the same
    # as required by application
    CURR_GIT_REPO_DIR :=$(CURR_COMPONENT_DIR)/openssl_git
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_OPENSSL_GIT_MANAGER_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    include $(CURR_COMPONENT_DIR)/openssl_git/Makefile.submodule.uc.mk

    KCONFIG_DIRS += OPENSSL_GIT_DIR=$(CURR_COMPONENT_DIR)/openssl_git
else
    KCONFIG_DIRS += OPENSSL_GIT_DIR=$(KCONFIG_DUMMY)
endif
