
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_NGHTTP2)

ifdef CONFIG_INCLUDE_NGHTTP2

    NGHTTP2_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/nghttp2
    ifeq ("$(wildcard $(NGHTTP2_PATH))","")
        $(info   )
        $(info --- NGHTTP2 path $(NGHTTP2_PATH) dont exists )
        $(info --- get repo from andew zamansky or from https://github.com/nghttp2/nghttp2.git  )
        $(info --- make sure that .git directory is located in $(NGHTTP2_PATH)/  after unpacking   )
        $(error )
    endif

    ifeq ($(MAKECMDGOALS),all_after_makefile_generated)
        ifndef NGHTTP2_GIT_TEST_ALREADY_PERFORMED
            NGHTTP2_GIT_TEST_ALREADY_PERFORMED:=1
            #test if current commit and branch of uboot git is the same as required by application
            CURR_GIT_REPOSITORY_DIR :=$(NGHTTP2_PATH)
            CURR_GIT_COMMIT_HASH_VARIABLE :=CONFIG_NGHTTP2_GIT_COMMIT_HASH
            include $(MAKEFILE_DEFS_ROOT_DIR)/_common_include_functions/git_prebuild_repo_check.mk
        endif
    endif
    
    CURR_NGHTTP2_COMPONENT_LOCATION := $(patsubst %/Makefile.uc.mk,%,$(realpath $(filter %nghttp2/Makefile.uc.mk,$(MAKEFILE_LIST))))
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_NGHTTP2_COMPONENT_LOCATION)/includes)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(CURR_NGHTTP2_COMPONENT_LOCATION)/includes/nghttp2)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(NGHTTP2_PATH)/lib/includes)
    
    
    
endif
#DEFINES =


#ASMFLAGS =

#INCLUDE_DIR =

SRC := nghttp2_session.c
SRC += nghttp2_callbacks.c
SRC += nghttp2_submit.c
SRC += nghttp2_hd.c
SRC += nghttp2_helper.c
SRC += nghttp2_priority_spec.c
SRC += nghttp2_version.c
SRC += nghttp2_mem.c
SRC += nghttp2_map.c
SRC += nghttp2_buf.c
SRC += nghttp2_frame.c
SRC += nghttp2_outbound_item.c
SRC += nghttp2_pq.c
SRC += nghttp2_stream.c
SRC += nghttp2_http.c
SRC += nghttp2_rcbuf.c
SRC += nghttp2_hd_huffman.c
SRC += nghttp2_hd_huffman_data.c

SRC += nghttp2_debug.c

VPATH += | $(NGHTTP2_PATH)/lib

include $(COMMON_CC)
