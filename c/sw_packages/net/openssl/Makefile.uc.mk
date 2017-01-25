
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_OPENSSL)

ifeq ($(strip $(CONFIG_INCLUDE_OPENSSL)),y)

    OPENSSL_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/openssl
    ifeq ("$(wildcard $(OPENSSL_PATH))","")
        $(info   )
        $(info --- OpenSSL path $(OPENSSL_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/openssl/openssl  )
        $(info --- make sure that .git directory is located in $(OPENSSL_PATH)/  after unpacking   )
        $(error )
    endif

    ifeq ($(MAKECMDGOALS),all_after_makefile_generated)
        ifndef OPENSSL_GIT_TEST_ALREADY_PERFORMED
            OPENSSL_GIT_TEST_ALREADY_PERFORMED:=1
            #test if current commit and branch of uboot git is the same as required by application
            CURR_GIT_REPOSITORY_DIR :=$(OPENSSL_PATH)
            CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_OPENSSL_GIT_COMMIT_HASH
            include $(MAKEFILE_DEFS_ROOT_DIR)/git_prebuild_repo_check.mk
        endif
    endif
    
    CURR_OPENSSL_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %openssl/Makefile.uc.mk,$(MAKEFILE_LIST))))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_OPENSSL_COMPONENT_LOCATION)/include)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_OPENSSL_COMPONENT_LOCATION)/include/openssl)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(OPENSSL_PATH)/include)
    
    
   
endif
#DEFINES =


#ASMFLAGS =

#INCLUDE_DIR =


#SRC = easy.c
#VPATH += | $(CURL_PATH)/lib/vtls


include $(COMMON_CC)
