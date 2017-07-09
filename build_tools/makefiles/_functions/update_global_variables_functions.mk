define ADD_TO_GLOBAL_INCLUDE_PATH =
    $(eval GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , path/to/include/dir )

define ADD_TO_GLOBAL_DEFINES =
    $(eval GLOBAL_DEFINES := $(GLOBAL_DEFINES) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , var )
#    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , var=val )

define ADD_TO_GLOBAL_LIBRARIES =
    $(eval GLOBAL_LIBS := $(GLOBAL_LIBS) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_LIBRARIES , var )

define ADD_TO_GLOBAL_LIBRARIES_PATH =
    $(eval GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_LIBRARIES_PATH , var )

define ADD_TO_GLOBAL_ARCHIVES =
    $(eval GLOBAL_ARCHIVES := $(GLOBAL_ARCHIVES) $(1))
endef
#usage :
#    DUMMY := $(call ADD_TO_GLOBAL_ARCHIVES , var )
