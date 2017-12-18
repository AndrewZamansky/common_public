
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_CURL)

ifdef CONFIG_INCLUDE_CURL

    CURR_CURL_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %curl/Makefile.uc.mk,$(MAKEFILE_LIST))))
    CURL_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/curl
    ifeq ("$(wildcard $(CURL_PATH))","")
        $(info   )
        $(info --- CURL path $(CURL_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/curl/curl  )
        $(info --- make sure that .git directory is located in $(CURL_PATH)/  after unpacking   )
        $(error )
    endif

    #test if current commit and branch of curl git is the same as required by application
    CURR_GIT_REPO_DIR :=$(CURL_PATH)
    CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_CURL_GIT_COMMIT_HASH
    CURR_GIT_BUNDLE :=$(CURR_CURL_COMPONENT_LOCATION)/curl.bundle
    include $(MAKEFILES_ROOT_DIR)/_include_functions/git_prebuild_repo_check.mk
    
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_CURL_COMPONENT_LOCATION)/include)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_CURL_COMPONENT_LOCATION)/include/curl)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURL_PATH)/include)
    
    

    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)    
        ifeq ($(strip $(CONFIG_MICROSOFT_COMPILER)),y)
            DUMMY := $(call ADD_TO_GLOBAL_LIBRARIES , Ws2_32.lib )
            DUMMY := $(call ADD_TO_GLOBAL_LIBRARIES_PATH , var )
        endif
    endif
    
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , CURL_STATICLIB )
    ifeq ($(strip $(CONFIG_CURL_USE_CUSTOM_CURL_CONFIG_H)),y)
        DUMMY := $(call ADD_TO_GLOBAL_DEFINES , HAVE_CONFIG_H )
    endif

endif


ifneq ($(strip $(CONFIG_CURL_HTTP_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_HTTP
endif
ifneq ($(strip $(CONFIG_CURL_FTP_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_FTP
endif
ifneq ($(strip $(CONFIG_CURL_TELNET_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_TELNET
endif
ifneq ($(strip $(CONFIG_CURL_DICT_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_DICT
endif
ifneq ($(strip $(CONFIG_CURL_TFTP_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_TFTP
endif
ifneq ($(strip $(CONFIG_CURL_FILE_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_FILE
endif
ifneq ($(strip $(CONFIG_CURL_LDAP_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_LDAP
endif
ifneq ($(strip $(CONFIG_CURL_IMAP_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_IMAP
endif
ifneq ($(strip $(CONFIG_CURL_SMTP_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_SMTP
endif
ifneq ($(strip $(CONFIG_GOPHER_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_GOPHER
endif
ifneq ($(strip $(CONFIG_CURL_SMB_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_SMB
endif
ifneq ($(strip $(CONFIG_CURL_POP3_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_POP3
endif
ifneq ($(strip $(CONFIG_CURL_RTSP_PROTOCOL_SUPPORT)),y)
    DEFINES += CURL_DISABLE_RTSP
endif
   
ifneq ($(strip $(CONFIG_CURL_ENABLE_PROXY)),y)
    DEFINES += CURL_DISABLE_PROXY
endif

ifneq ($(strip $(CONFIG_CURL_ENABLE_COOKIES)),y)
    DEFINES += CURL_DISABLE_COOKIES
endif

ifneq ($(strip $(CONFIG_CURL_ENABLE_VERBOSE_STRINGS)),y)
    DEFINES += CURL_DISABLE_VERBOSE_STRINGS
else
    DEFINES += Curl_nop_stmt=printf
endif

ifeq ($(strip $(CONFIG_CURL_ENABLE_DEBUG)),y)
    DEFINES += DEBUGBUILD
endif

ifeq ($(strip $(CONFIG_CURL_USE_OPENSSL)),y)
    DEFINES += USE_OPENSSL
endif


ifeq ($(strip $(CONFIG_CURL_USE_WOLFSSL)),y)
    DEFINES += USE_CYASSL
endif

ifeq ($(strip $(CONFIG_CURL_USE_NGHTTP2)),y)
    DEFINES += USE_NGHTTP2
endif

ifneq ($(strip $(CONFIG_CURL_ENABLE_CRYPTO_AUTHENTICATION)),y)
    DEFINES += CURL_DISABLE_CRYPTO_AUTH
endif

ifneq ($(strip $(CONFIG_CURL_DEFAULT_RECIEVE_BUFFER_SIZE)),y)#if NOT defined
    DEFINES += CURL_MAX_WRITE_SIZE=$(CONFIG_CURL_RECIEVE_BUFFER_SIZE)
endif

ifneq ($(strip $(CONFIG_CURL_DEFAULT_HTTP2_BUFFER_SIZE)),y)#if NOT defined
    DEFINES += H2_BUFSIZE=$(CONFIG_CURL_HTTP2_BUFFER_SIZE)
endif

ifeq ($(strip $(CONFIG_GCC)),y)
    DEFINES += HAVE_STRTOLL
endif

DEFINES += CURL_STRICTER
ifdef CONFIG_MICROSOFT_COMPILER
    CFLAGS += /wd4127 #disable warning C4127: conditional expression is constant
    DEFINES += _CRT_SECURE_NO_WARNINGS
endif
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

# followin files compiled always but contets of each are used 
# according to following defines :  CURLRES_ARES , CURLRES_THREADED , CURLRES_SYNCH
SRC += hostsyn.c
SRC += asyn-thread.c
SRC += asyn-ares.c

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)    
    SRC += system_win32.c
    SRC += strtok.c
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
