
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_CURL)

ifdef CONFIG_INCLUDE_CURL

    CURL_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/curl
    ifeq ("$(wildcard $(CURL_PATH))","")
        $(info   )
        $(info --- CURL path $(CURL_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/curl/curl  )
        $(info --- make sure that .git directory is located in $(CURL_PATH)/  after unpacking   )
        $(error )
    endif

    ifeq ($(MAKECMDGOALS),all_after_makefile_generated)
        ifndef CURL_GIT_TEST_ALREADY_PERFORMED
            CURL_GIT_TEST_ALREADY_PERFORMED:=1
            #test if current commit and branch of uboot git is the same as required by application
            CURR_GIT_REPOSITORY_DIR :=$(CURL_PATH)
            CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_CURL_GIT_COMMIT_HASH
            include $(MAKEFILE_DEFS_ROOT_DIR)/_common_include_functions/git_prebuild_repo_check.mk
        endif
    endif
    
    CURR_CURL_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %curl/Makefile.uc.mk,$(MAKEFILE_LIST))))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_CURL_COMPONENT_LOCATION)/include)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_CURL_COMPONENT_LOCATION)/include/curl)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURL_PATH)/include)
    
    
    ifneq ($(strip $(CONFIG_CURL_HTTP_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_HTTP )
    endif
    ifneq ($(strip $(CONFIG_CURL_FTP_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_FTP )
    endif
    ifneq ($(strip $(CONFIG_CURL_TELNET_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_TELNET )
    endif
    ifneq ($(strip $(CONFIG_CURL_DICT_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_DICT )
    endif
    ifneq ($(strip $(CONFIG_CURL_TFTP_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_TFTP )
    endif
    ifneq ($(strip $(CONFIG_CURL_FILE_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_FILE )
    endif
    ifneq ($(strip $(CONFIG_CURL_LDAP_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_LDAP )
    endif
    ifneq ($(strip $(CONFIG_CURL_IMAP_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_IMAP )
    endif
    ifneq ($(strip $(CONFIG_CURL_SMTP_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_SMTP )
    endif
    ifneq ($(strip $(CONFIG_GOPHER_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_GOPHER )
    endif
    ifneq ($(strip $(CONFIG_CURL_SMB_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_SMB )
    endif
    ifneq ($(strip $(CONFIG_CURL_POP3_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_POP3 )
    endif
    ifneq ($(strip $(CONFIG_CURL_RTSP_PROTOCOL_SUPPORT)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_RTSP )
    endif
       
    ifneq ($(strip $(CONFIG_CURL_ENABLE_PROXY)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_PROXY )
    endif

    ifneq ($(strip $(CONFIG_CURL_ENABLE_COOKIES)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_COOKIES )
    endif

    ifneq ($(strip $(CONFIG_CURL_CURL_ENABLE_VERBOSE_STRINGS)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_DISABLE_VERBOSE_STRINGS )
    endif

    ifeq ($(strip $(CONFIG_CURL_USE_OPENSSL)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , USE_OPENSSL )
    endif

    ifeq ($(strip $(CONFIG_CURL_USE_WOLFSSL)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , USE_CYASSL )
    endif

    ifeq ($(strip $(CONFIG_CURL_USE_NGHTTP2)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , USE_NGHTTP2 )
    endif

    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_STRICTER )

    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)    
        ifeq ($(strip $(CONFIG_MICROSOFT_COMPILER)),y)
            DUMMY := $(call ADD_TO_GLOBAL_LIBRARIES , Ws2_32.lib )
            DUMMY := $(call ADD_TO_GLOBAL_LIBRARIES_PATH , var )
        endif
    endif
   
endif
DEFINES :=


#ASMFLAGS =

#INCLUDE_DIR =


SRC = easy.c
SRC += url.c
SRC += strcase.c
SRC += getenv.c
SRC += escape.c
SRC += slist.c
SRC += llist.c
SRC += hash.c
SRC += timeval.c
SRC += asyn-thread.c
SRC += curl_addrinfo.c
SRC += multi.c
SRC += hostasyn.c
SRC += sendf.c
SRC += inet_pton.c
SRC += curl_threads.c
SRC += connect.c
SRC += mprintf.c
SRC += hostip.c
SRC += getinfo.c
SRC += select.c
SRC += conncache.c
SRC += if2ip.c
SRC += strerror.c
SRC += nonblock.c
SRC += inet_ntop.c
SRC += progress.c
SRC += warnless.c
SRC += version.c
SRC += strdup.c
SRC += share.c
SRC += hostip4.c
SRC += splay.c
SRC += wildcard.c
SRC += transfer.c
SRC += speedcheck.c
SRC += pipeline.c
SRC += netrc.c
SRC += dotdot.c
SRC += fileinfo.c
SRC += strtok.c

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)    
    SRC += system_win32.c
endif

ifeq ($(strip $(CONFIG_CURL_HTTP_PROTOCOL_SUPPORT)),y)
    SRC += http.c
    SRC += http_digest.c
    SRC += http_ntlm.c
    SRC += http_chunks.c
    SRC += base64.c
    SRC += md5.c
    SRC += rand.c
    SRC += parsedate.c
    SRC += formdata.c
    SRC += hmac.c
    SRC += curl_endian.c
    SRC += curl_ntlm_core.c
    SRC += curl_des.c
    SRC += curl_gethostname.c
    
    SRC += digest.c
    SRC += vauth.c
    SRC += ntlm.c
    VPATH += | $(CURL_PATH)/lib/vauth
endif
VPATH += | $(CURL_PATH)/lib

    
INCLUDE_DIR +=$(CURL_PATH)/lib
ifeq ($(strip $(CONFIG_CURL_USE_OPENSSL)),y)
    SRC += openssl.c
    SRC += hostcheck.c
endif

ifeq ($(strip $(CONFIG_CURL_USE_WOLFSSL)),y)
    DEFINES += HAVE_CYASSL_ERROR_SSL_H
    SRC += cyassl.c
endif

ifeq ($(strip $(CONFIG_CURL_USE_NGHTTP2)),y)
    SRC += http2.c
endif

SRC += vtls.c
VPATH += | $(CURL_PATH)/lib/vtls


include $(COMMON_CC)
