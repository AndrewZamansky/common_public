

# test for valid target 
VALID_TARGETS := all clean rebuild archive list_var all_after_makefile_generated archive_step2 menuconfig
ifeq ($(findstring $(MAKECMDGOALS),$(VALID_TARGETS)),) 

default : 
	$(info  )
	$(info No valid targect selected . available targets listed below :  )
	$(info  )
	$(info "make all"       :  build whole project  )
	$(info "make clean"     :  delete objects , project outputs  )
	$(info "make rebuild"   :  delete objects , project outputs and build whole project  )
	$(info "make menuconfig":  create configuration file  )
	$(info "make archive"   :  export source code to archive   )
	$(info "make list_var"  :  print usefull makefile variables   )
	$(info  )
	$(info  )
	
$(MAKECMDGOALS) : default
	$(info  ) 

else # valid target :


COMMON_CC :=

include $(MAKEFILE_DEFS_ROOT_DIR)/common_init.mk

include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk


ifeq ($(wildcard $(OBJ_DIR)),) 		#if $(OBJ_DIR) dont exists then $(wildcard $(OBJ_DIR)) will produce empty string 
   DUMMY:=$(shell $(MKDIR)  $(OBJ_DIR)) # create   $(OBJ_DIR)
endif
ifeq ($(wildcard $(OUT_DIR)),) 		#if $(OUT_DIR) dont exists then $(wildcard $(OUT_DIR)) will produce empty string 
   DUMMY:=$(shell $(MKDIR)  $(OUT_DIR))  # create   $(OUT_DIR)
endif

ifeq ($(wildcard $(OUT_DIR_HISTORY)),) 		#if $(OUT_DIR) dont exists then $(wildcard $(OUT_DIR)) will produce empty string 
   DUMMY:=$(shell $(MKDIR)  $(OUT_DIR_HISTORY))  # create   $(OUT_DIR)
endif

ifeq ($(wildcard $(AUTO_GENERATED_FILES_DIR)),) 		#if $(OUT_DIR) dont exists then $(wildcard $(OUT_DIR)) will produce empty string 
   DUMMY:=$(shell $(MKDIR)  $(AUTO_GENERATED_FILES_DIR))  # create   $(OUT_DIR)
endif


ifdef MAKEFILE_WAS_GENERATED
	ifndef SKIP_SECTION_THAT_SHOULD_RUN_ONCE_AFTER_AUTO_FILE_GENERATIONS
        include $(COMPONENTS_MK)
        # clean following variables befor exporting to sub-makes
        DEFINES :=
        INCLUDE_DIR :=
        SRC :=
        CFLAGS :=
        ASMFLAGS :=
        DEFINES :=
        
        # move global prerequirements_check compilation to first place
        SUBDIRS := $(filter %prerequirements_check,$(SUBDIRS))  $(filter-out %prerequirements_check,$(SUBDIRS))

        GLOBAL_HEADER_FILES_DEPS := $(patsubst %,%/*.h,$(GLOBAL_INCLUDE_DIR)) 

        ADDITIONAL_DIRS_FOR_EXPORTS := $(sort $(APP_ROOT_DIR) $(SUBDIRS) $(GLOBAL_INCLUDE_DIR) $(MAKEFILE_DEFS_ROOT_DIR))
        ADDITIONAL_DIRS_FOR_EXPORTS += $(BUILD_TOOLS_ROOT_DIR)/scatter_files
        ADDITIONAL_DIRS_FOR_EXPORTS := $(patsubst $(APP_ROOT_DIR)/%,%,$(realpath $(ADDITIONAL_DIRS_FOR_EXPORTS)))
        ADDITIONAL_DIRS_FOR_EXPORTS :=  $(patsubst $(WORKSPACE_ROOT_DIR)/%,$(WORKSPACE_NAME)/%,$(ADDITIONAL_DIRS_FOR_EXPORTS))#macke folder relative to workspace


		include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk #update all variables that use new generated files

        #remove duplicate entries :
        GLOBAL_INCLUDE_DIR := $(sort $(GLOBAL_INCLUDE_DIR))
        GLOBAL_DEFINES := $(sort $(GLOBAL_DEFINES))
        GLOBAL_LIBS := $(sort $(GLOBAL_LIBS))
        GLOBAL_LIBS_PATH := $(sort $(GLOBAL_LIBS_PATH))
        
        SKIP_SECTION_THAT_SHOULD_RUN_ONCE_AFTER_AUTO_FILE_GENERATIONS = YES
		export #export all variables declared before
	endif
endif 


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
	ARCHIVER :=	cd $(WORKSPACE_ROOT_DIR)/.. &"$(ARCHIVER_PROGRAM_DIR)/7z" a -xr!*OBJ -xr!*OUT*
endif



all: $(COMPONENTS_MK)
	$(eval MAKEFILE_WAS_GENERATED := YES)
	$(eval export MAKEFILE_WAS_GENERATED)
	$(MAKE) -f $(MAKEFILE_DEFS_ROOT_DIR)/prebuild_check.mk
	$(MAKE) all_after_makefile_generated

all_after_makefile_generated : build_outputs
	$(eval TARGET := all)
	$(info ---- Compiling project $(APP_ROOT_DIR) done at $(TIME_STR) ----)

rebuild : clean
	$(MAKE) all
	
archive : 
	$(eval CREATING_ARCHIVE := YES)
	$(eval export CREATING_ARCHIVE)
	$(eval TARGET := archive)
	$(ARCHIVER) $(ARCHIVE_OUTPUT) $(ADDITIONAL_DIRS_FOR_EXPORTS)
	$(MAKE) archive_step2
	$(info ----archive done ----)

archive_step2 : $(SUBDIRS)

$(SUBDIRS):
	$(eval COMMON_CC := $(MAKEFILE_DEFS_ROOT_DIR)/common_cc.mk)
	$(eval export COMMON_CC)
	$(eval export TARGET)
	$(MAKE) -C $@ -f Makefile.uc.mk $(TARGET)

$(COMPONENTS_MK) : $(APP_ROOT_DIR)/.config
	$(MAKE) -f $(MAKEFILE_DEFS_ROOT_DIR)/generate_project_files.mk 
	
	
build_outputs :  $(SUBDIRS)
	$(MAKE) -f $(MAKEFILE_DEFS_ROOT_DIR)/build_final_outputs.mk 


clean:
	$(RM) $(OBJ_DIR)
	$(RM) $(OUT_DIR)
	$(RM) $(AUTO_GENERATED_FILES_DIR)
	
	
list_var:
	$(MAKE) -f $(MAKEFILE_DEFS_ROOT_DIR)/list_usful_variables.mk  



menuconfig:
	$(MAKE) -f $(MAKEFILE_DEFS_ROOT_DIR)/menuconfig.mk  
	
.PHONY: default rebuild archive clean all $(SUBDIRS)

endif# test for valid target