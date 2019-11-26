

GLOBAL_DEFINES :=
GLOBAL_INCLUDE_DIR :=
SUBDIRS :=
LIBS :=
WHOLE_LIBS :=
LIBRARIES_DIRS :=
LDS_PREPROCESSOR_FLAGS :=

GLOBAL_CFLAGS :=
GLOBAL_ASMFLAGS :=
GLOBAL_LDFLAGS :=

include $(MAKEFILES_ROOT_DIR)/common.mk

include $(MAKEFILES_ROOT_DIR)/c/init_for_c.mk


ALL_COMPONENT_MAKEFILES :=
include $(COMPONENTS_MK)
ALL_COMPONENTS_MAKEFILES :=$(patsubst %,%/Makefile.uc.mk, $(SUBDIRS))
include $(ALL_COMPONENTS_MAKEFILES)
DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(AUTO_GLOBAL_INCLUDE_DIRS))

# versioning should be after "include $(COMPONENTS_MK)"
# because of git modifications checking
include $(PREBUILD_ROUTINES_DIR)/init_versions.mk

# clean following variables befor exporting to sub-makes
DEFINES :=
INCLUDE_DIR :=
SRC :=
CFLAGS :=
ASMFLAGS :=
DEFINES :=

# move global prerequirements_check compilation to first place
SUBDIRS :=$(sort $(SUBDIRS))
TEMP :=$(filter %prerequirements_check,$(SUBDIRS))
SUBDIRS :=$(TEMP) $(filter-out %prerequirements_check,$(SUBDIRS))

GLOBAL_HEADER_FILES_DEPS := $(patsubst %,%/*.h,$(GLOBAL_INCLUDE_DIR))
GLOBAL_HEADER_FILES_DEPS := $(wildcard $(GLOBAL_HEADER_FILES_DEPS))

#remove duplicate entries :
GLOBAL_INCLUDE_DIR := $(sort $(GLOBAL_INCLUDE_DIR))
GLOBAL_DEFINES := $(sort $(GLOBAL_DEFINES))
GLOBAL_LIBS := $(sort $(GLOBAL_LIBS))
GLOBAL_LIBS_PATH := $(sort $(GLOBAL_LIBS_PATH))
GLOBAL_ARCHIVES := $(sort $(GLOBAL_ARCHIVES))

# it looks like TMP variable is special variable for makefile
# it should contain folder that make will put temporary files in it
TMP :=$(MAKE_TEMPORARY_FILES_DIR)
export #export all variables declared before

SUBDIRS_LAST := $(filter %auto_generated_command_list,$(SUBDIRS))
SUBDIRS := $(filter-out %auto_generated_command_list,$(SUBDIRS))

build_all : build_outputs
	$(MAKE) -r -f $(MAKEFILES_ROOT_DIR)/c/post_build.mk

ifeq (,$(strip $(SUBDIRS_LAST)))

build_outputs: $(SUBDIRS)
	$(MAKE) -r -f $(MAKEFILES_ROOT_DIR)/c/build_final_outputs.mk

else

build_outputs: $(SUBDIRS_LAST)
	$(MAKE) -r -f $(MAKEFILES_ROOT_DIR)/c/build_final_outputs.mk

$(SUBDIRS_LAST): $(SUBDIRS)
	$(eval COMMON_CC := $(MAKEFILES_ROOT_DIR)/c/compilers/common_cc.mk)
	$(eval SRC :=)
	$(eval VPATH :=)
	$(eval CFLAGS :=)
	$(eval DEFINES :=)
	$(eval ASMFLAGS :=)
	$(eval export COMMON_CC)
	$(eval export SRC)
	$(eval export CFLAGS)
	$(eval export DEFINES)
	$(eval export ASMFLAGS)
	$(eval export VPATH)
	$(info - $(patsubst $(PARENT_OF_COMMON_PUBLIC_DIR)/%,%,$@))
	$(MAKE) -r -C $@ -f Makefile.uc.mk all

endif

$(SUBDIRS):
	$(eval COMMON_CC := $(MAKEFILES_ROOT_DIR)/c/compilers/common_cc.mk)
	$(eval SRC :=)
	$(eval VPATH :=)
	$(eval CFLAGS :=)
	$(eval DEFINES :=)
	$(eval ASMFLAGS :=)
	$(eval export COMMON_CC)
	$(eval export SRC)
	$(eval export CFLAGS)
	$(eval export DEFINES)
	$(eval export ASMFLAGS)
	$(eval export VPATH)
	$(info - $(patsubst $(PARENT_OF_COMMON_PUBLIC_DIR)/%,%,$@))
	$(MAKE) -r -C $@ -f Makefile.uc.mk all


.PHONY: default  archive clean all $(SUBDIRS) $(SUBDIRS_LAST)
