ifndef COMMON_MK
    $(error   you should include common.mk instead of this file)
endif

# helper function to append string
define _APPEND_VAR =
    $(if $(1),$(eval $(1) :=$($(1))$(2)),)
endef
# helper function to append string only if such string doesn't exist.
# spaces in ' $($(1)) ' are important
define _APPEND_IF_NOT_EXISTS =
    $(if $(findstring $(2), $($(1)) ),,$(call _APPEND_VAR,$(1),$(2)))
endef

# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_INCLUDE_PATH =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_INCLUDE_DIR, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_INCLUDE_PATH, path/to/include/dir))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_DEFINES =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_DEFINES, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_DEFINES, var))
#    $(eval $(call ADD_TO_GLOBAL_DEFINES, var=val))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_LIBS =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_LIBS, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_LIBS, var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_WHOLE_LIBS =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_WHOLE_LIBS, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_WHOLE_LIBS, var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_CFLAGS =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_CFLAGS, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_CFLAGS, var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_LDFLAGS =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_LDFLAGS, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_LDFLAGS, var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_LIBS_PATH =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_LIBS_PATH, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_LIBS_PATH, var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_ARCHIVES =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_ARCHIVES, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_ARCHIVES, var))


# function to set folder that contains Makefile.postbuild.mk postbuild makefile
# POST_BUILD_MAKEFILE_DIR must contain at most 1 such folder

check_for_same_post_build_folder =$(filter-out $(1),$(POST_BUILD_MAKEFILE_DIR))
define _SET_POSTBUILD_STEP_1 =
    $(if $(1),\
        $(if $(call check_for_same_post_build_folder,$(1)),\
            $(info err: POST_BUILD_MAKEFILE_DIR may contain only 1 folder)\
            $(info err: POST_BUILD_MAKEFILE_DIR already contains folder)\
            $(info err: POST_BUILD_MAKEFILE_DIR=$(POST_BUILD_MAKEFILE_DIR))\
            $(call exit,1),\
            $(eval POST_BUILD_MAKEFILE_DIR:=$(1)) ),\
        do nothing)
endef

define SET_DIR_WITH_POSTBUILD_MAKEFILE =
    $(call _SET_POSTBUILD_STEP_1,$(strip $(1)))
endef
#usage :
#    DUMMY := $(call SET_DIR_WITH_POSTBUILD_MAKEFILE, var))
