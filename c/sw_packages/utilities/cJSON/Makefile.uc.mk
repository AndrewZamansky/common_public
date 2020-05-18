
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_CJSON)

ifeq ($(strip $(CONFIG_INCLUDE_CJSON)),y)

    CJSON_GIT :=https://github.com/DaveGamble/cJSON.git
    CJSON_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/cJSON
    ifeq ("$(wildcard $(CJSON_PATH))","")
        $(info   )
        $(info !--- cJSON path $(CJSON_PATH) does not exist)
        $(info !--- get repo from andew zamansky or from $(CJSON_GIT))
        $(info !--- make sure that .git directory is)
        $(info !--- located in $(CJSON_PATH)/  after unpacking)
        $(error )
    endif

    # test if current commit and branch of cJSON git is
    # the same as required by application
    CURR_GIT_REPO_DIR :=$(CJSON_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_CJSON_GIT_COMMIT_HASH
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk

    ifeq ("","$(filter $(CJSON_PATH),$(EXTERNAL_SRC_DIRS))")
        EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(CJSON_PATH)
    endif


    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CJSON_PATH))


endif

DEFINES :=
CFLAGS :=
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ifeq ($(strip $(CONFIG_MICROSOFT_COMPILER)),y)
        # disable warning C4232: nonstandard extension used : 'cJSON_malloc' :
        # address of dllimport 'malloc' is not static, identity not guaranteed
        CFLAGS := /wd4232

        # disable warning C4706: assignment within conditional expression
        CFLAGS := /wd4706
        DEFINES += _CRT_SECURE_NO_WARNINGS
    endif
endif

SRC := cJSON.c
VPATH += | $(CJSON_PATH)


include $(COMMON_CC)
