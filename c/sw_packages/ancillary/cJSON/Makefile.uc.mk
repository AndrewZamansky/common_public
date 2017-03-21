
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_CJSON)

ifeq ($(strip $(CONFIG_INCLUDE_CJSON)),y)

    CJSON_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/cJSON
    ifeq ("$(wildcard $(CJSON_PATH))","")
        $(info   )
        $(info --- cJSON path $(CJSON_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/kbranigan/cJSON.git  )
        $(info --- make sure that .git directory is located in $(CJSON_PATH)/  after unpacking   )
        $(error )
    endif


    #CURR_CJSON_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %cJSON/Makefile.uc.mk,$(MAKEFILE_LIST))))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CJSON_PATH))


endif

DEFINES :=
CFLAGS :=

ifdef CONFIG_MICROSOFT_COMPILER
    CFLAGS := /wd4232 #disable warning C4232: nonstandard extension used : 'cJSON_malloc' : address of dllimport 'malloc' is not static, identity not guaranteed
    CFLAGS := /wd4706 #disable warning C4706: assignment within conditional expression
    DEFINES += _CRT_SECURE_NO_WARNINGS
endif

SRC := cJSON.c
VPATH += | $(CJSON_PATH)


include $(COMMON_CC)
