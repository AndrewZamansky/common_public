
SVN_ROOT_DIR:=

##### test for existence of svn and put its directory name in SVN_ROOT_DIR #####
SEARCHED_TOOL:=svn
SEARCHED_DIR_VARIABLE:=SVN_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_SVN_ROOT_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=svn.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=svn
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    SVN ="$(SVN_ROOT_DIR)\svn.exe"
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    SVN ="$(SVN_ROOT_DIR)/svn.exe"
endif



## test for validity of application svn repository
SVN_DIR := $(firstword $(wildcard ./.svn))
ifeq ($(findstring ./.svn,$(SVN_DIR)),)    # if not found ./.svn in $(SVN_DIR)
    $(info err: svn project repository not found.)
    $(info ---: checkout project svn to project directory,)
    $(info ---: or advise with svn repo manger, import and then checkout.)
    $(call exit,1)
endif


# test if current commit and branch of common_public
# is the same as required by application
CURR_SVN_REPO_DIR :=$(COMMON_PUBLIC_DIR)
CURR_SVN_REVISION_VARIABLE :=CONFIG_COMMON_PUBLIC_SVN_REVISION
include $(MAKEFILES_ROOT_DIR)/_include_functions/svn_prebuild_repo_check.mk


ifdef CONFIG_USE_COMMON_PRIVATE_PACKAGES
    # test if current commit and branch of common_private
    # is the same as required by application
    CURR_SVN_REPO_DIR :=$(COMMON_PRIVATE_DIR)
    CURR_SVN_REVISION_VARIABLE :=CONFIG_COMMON_PRIVATE_SVN_REVISION
    include $(MAKEFILES_ROOT_DIR)/_include_functions/svn_prebuild_repo_check.mk
else
    $(info ---- common private packages not in use)
endif
