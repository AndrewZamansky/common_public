
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_WOLFSSL)

ifeq ($(strip $(CONFIG_INCLUDE_WOLFSSL)),y)

    WOLFSSL_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/wolfssl
    ifeq ("$(wildcard $(WOLFSSL_PATH))","")
        $(info   )
        $(info --- wolfSSL path $(WOLFSSL_PATH) dont exists )
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
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(WOLFSSL_PATH))
#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(WOLFSSL_PATH)/crypto/modes)

#    ifneq ($(strip $(CONFIG_WOLFSSL_MD5)),y)
#        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , WOLFSSL_NO_MD5 )
#    endif



endif

DEFINES :=
CFLAGS :=

ifdef CONFIG_MICROSOFT_COMPILER
    CFLAGS += /wd4127 #disable warning C4127: conditional expression is constant
    CFLAGS += /wd4204 #disable warning C4204: nonstandard extension used : non-constant aggregate initializer
    CFLAGS += /wd4214 #disable warning C4214: nonstandard extension used : bit field types other than int
    DEFINES += _CRT_SECURE_NO_WARNINGS
endif

DEFINES += NO_DSA  NO_MD4 NO_PSK  WC_NO_RSA_OAEP NO_WOLFSSL_SERVER NO_ERROR_STRINGS
DEFINES +=  NO_RC4 NO_RABBIT NO_HC128  SMALL_SESSION_CACHE  WOLFSSL_SMALL_STACK


DEFINES += WOLFSSL_STATIC_RSA
DEFINES += NO_DES3 NO_WRITEV

#DEFINES += MAX_CHAIN_DEPTH=16
SRC :=
SRC += ssl.c

ifneq ($(strip $(CONFIG_WOLFSSL_TLS)),y)
    DEFINES += NO_TLS
else
    SRC += tls.c
endif

ifneq ($(strip $(CONFIG_WOLFSSL_RSA)),y)
    DEFINES += NO_RSA
else
    SRC += rsa.c
endif


ifeq ($(strip $(CONFIG_WOLFSSL_ECC)),y)
    DEFINES += HAVE_ECC
    SRC += ecc.c
endif

ifeq ($(strip $(CONFIG_WOLFSSL_AES)),y)
    SRC += aes.c
endif

ifeq ($(strip $(CONFIG_WOLFSSL_DHE_RSA_WITH_AES_128_GCM_SHA256)),y)
    DEFINES += HAVE_AESGCM
    #DEFINES += BUILD_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256  # no need to define . auto defined with HAVE_AESGCM 
endif



ifneq ($(strip $(CONFIG_WOLFSSL_MD5)),y)
    DEFINES += NO_MD5
else
    SRC += md5.c
endif

ifneq ($(strip $(CONFIG_WOLFSSL_SHA256)),y)
    DEFINES += NO_SHA256
else
    SRC += sha256.c
endif

ifneq ($(strip $(CONFIG_WOLFSSL_DH)),y)
    DEFINES += NO_DH
else
    SRC += dh.c
endif

ifneq ($(strip $(CONFIG_WOLFSSL_HMAC)),y)
    DEFINES += NO_HMAC
else
    SRC += hmac.c
endif

#SRC += pwd.c

#ASMFLAGS =

INCLUDE_DIR :=
INCLUDE_DIR +=

SRC += internal.c
SRC += keys.c
VPATH += | $(WOLFSSL_PATH)/src

SRC += random.c
SRC += memory.c
SRC += wc_port.c
SRC += asn.c
SRC += integer.c
SRC += io.c
SRC += sha.c
SRC += hash.c
SRC += coding.c
SRC += pwdbased.c
VPATH += | $(WOLFSSL_PATH)/wolfcrypt/src

SRC += wolfmath.c




include $(COMMON_CC)
