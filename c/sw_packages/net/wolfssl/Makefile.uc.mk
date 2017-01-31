
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_WOLFSSL)

ifeq ($(strip $(CONFIG_INCLUDE_WOLFSSL)),y)

    WOLFSSL_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/wolfssl
    ifeq ("$(wildcard $(WOLFSSL_PATH))","")
        $(info   )
        $(info --- OpenSSL path $(WOLFSSL_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/wolfSSL/wolfssl  )
        $(info --- make sure that .git directory is located in $(WOLFSSL_PATH)/  after unpacking   )
        $(error )
    endif

    ifeq ($(MAKECMDGOALS),all_after_makefile_generated)
        ifndef WOLFSSL_GIT_TEST_ALREADY_PERFORMED
            WOLFSSL_GIT_TEST_ALREADY_PERFORMED:=1
            #test if current commit and branch of uboot git is the same as required by application
            CURR_GIT_REPOSITORY_DIR :=$(WOLFSSL_PATH)
            CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_WOLFSSL_GIT_COMMIT_HASH
            include $(MAKEFILE_DEFS_ROOT_DIR)/_common_include_functions/git_prebuild_repo_check.mk
        endif
    endif

    CURR_WOLFSSL_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %wolfssl/Makefile.uc.mk,$(MAKEFILE_LIST))))
#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_WOLFSSL_COMPONENT_LOCATION)/include)
#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_WOLFSSL_COMPONENT_LOCATION)/include/openssl)
#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(WOLFSSL_PATH)/include)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(WOLFSSL_PATH))
#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(WOLFSSL_PATH)/crypto/modes)

#    ifneq ($(strip $(CONFIG_WOLFSSL_MD5)),y)
#        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , WOLFSSL_NO_MD5 )
#    endif

    DUMMY := $(call ADD_TO_GLOBAL_DEFINES ,NO_WOLFSSL_SERVER )


endif

DEFINES = NO_DSA NO_ERROR_STRINGS NO_MD4 NO_PSK  WC_NO_RSA_OAEP
DEFINES += NO_PWDBASED NO_RC4 NO_RABBIT NO_HC128  SMALL_SESSION_CACHE

ifneq ($(strip $(CONFIG_WOLFSSL_TLS)),y)
    DEFINES += NO_TLS
endif

ifneq ($(strip $(CONFIG_WOLFSSL_SHA256)),y)
    DEFINES += NO_SHA256
endif



#ASMFLAGS =

INCLUDE_DIR :=
INCLUDE_DIR +=

SRC :=
SRC += tls.c
SRC += ssl.c
SRC += internal.c
SRC += keys.c
VPATH += | $(WOLFSSL_PATH)/src

SRC += sha256.c
SRC += random.c
SRC += memory.c
SRC += wc_port.c
SRC += rsa.c
SRC += asn.c
SRC += integer.c
SRC += io.c
SRC += des3.c
SRC += sha.c
SRC += dh.c
SRC += hash.c
SRC += aes.c
SRC += coding.c
SRC += hmac.c
VPATH += | $(WOLFSSL_PATH)/wolfcrypt/src

ifneq ($(strip $(CONFIG_WOLFSSL_MD5)),y)
    DEFINES += NO_MD5
else
    SRC += md5.c
endif

ifneq ($(strip $(CONFIG_WOLFSSL_SHA256)),y)
    DEFINES += NO_SHA256
endif


include $(COMMON_CC)
