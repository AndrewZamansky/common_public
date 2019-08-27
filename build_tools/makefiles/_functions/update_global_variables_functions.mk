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
#    $(eval $(call ADD_TO_GLOBAL_INCLUDE_PATH , path/to/include/dir))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_DEFINES =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_DEFINES, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_DEFINES , var))
#    $(eval $(call ADD_TO_GLOBAL_DEFINES , var=val))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_LIBRARIES =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_LIBS, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_LIBRARIES , var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_CFLAGS =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_CFLAGS, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_CFLAGS , var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_LDFLAGS =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_LDFLAGS, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_LDFLAGS, var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_LIBRARIES_PATH =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_LIBS_PATH, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_LIBRARIES_PATH , var))


# spaces in ' $(strip $(1)) ' are important
define ADD_TO_GLOBAL_ARCHIVES =
    $(call _APPEND_IF_NOT_EXISTS,GLOBAL_ARCHIVES, $(strip $(1)) )
endef
#usage :
#    $(eval $(call ADD_TO_GLOBAL_ARCHIVES , var))
