

GLOBAL_DEFINES :=
GLOBAL_INCLUDE_DIR :=
SUBDIRS :=
LIBS :=
LIBRARIES_DIRS :=
LDS_PREPROCESSOR_FLAGS :=

GLOBAL_CFLAGS :=
GLOBAL_ASMFLAGS :=
GLOBAL_LDFLAGS :=

include $(MAKEFILES_ROOT_DIR)/common.mk

include $(MAKEFILES_ROOT_DIR)/c/init_for_c.mk




include $(COMPONENTS_MK)

# clean following variables befor exporting to sub-makes
DEFINES :=
INCLUDE_DIR :=
SRC :=
CFLAGS :=
ASMFLAGS :=
DEFINES :=

# move global prerequirements_check compilation to first place
TMP :=$(filter %prerequirements_check,$(SUBDIRS))
SUBDIRS :=$(TMP) $(sort $(filter-out %prerequirements_check,$(SUBDIRS)))

GLOBAL_HEADER_FILES_DEPS := $(patsubst %,%/*.h,$(GLOBAL_INCLUDE_DIR)) 

#remove duplicate entries :
GLOBAL_INCLUDE_DIR := $(sort $(GLOBAL_INCLUDE_DIR))
GLOBAL_DEFINES := $(sort $(GLOBAL_DEFINES))
GLOBAL_LIBS := $(sort $(GLOBAL_LIBS))
GLOBAL_LIBS_PATH := $(sort $(GLOBAL_LIBS_PATH))
GLOBAL_ARCHIVES := $(sort $(GLOBAL_ARCHIVES))

export #export all variables declared before



build_all : build_outputs
	$(MAKE) -f $(MAKEFILES_ROOT_DIR)/c/post_build.mk

build_outputs :  $(SUBDIRS)
	$(MAKE) -f $(MAKEFILES_ROOT_DIR)/c/build_final_outputs.mk


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
	$(eval export TARGET)
	@echo - $(patsubst $(PARENT_OF_COMMON_PUBLIC_DIR)/%,%,$@)
	$(MAKE) -C $@ -f Makefile.uc.mk $(TARGET)


.PHONY: default  archive clean all $(SUBDIRS)
