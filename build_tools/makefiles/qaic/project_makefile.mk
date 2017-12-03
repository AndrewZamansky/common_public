

# test for valid target 
VALID_TARGETS = all clean rebuild archive list_var
VALID_TARGETS += all_after_makefile_generated archive_step2 menuconfig
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


#this line is reached with only COMMON_PROJECT_MAKEFILE_DIR initialized
include $(COMMON_PROJECT_MAKEFILE_DIR)/../prebuild_routines/init_workspace.mk
include $(MAKEFILES_ROOT_DIR)/qaic/init_for_qaic.mk


include $(MAKEFILES_ROOT_DIR)/common.mk

$(call mkdir_if_not_exists, $(OUT_DIR))
$(call mkdir_if_not_exists, $(OUT_DIR_HISTORY))
$(call mkdir_if_not_exists, $(AUTO_GENERATED_FILES_DIR))


ifdef MAKEFILE_WAS_GENERATED
    ifndef SKIP_SECTION_THAT_SHOULD_RUN_ONCE_AFTER_AUTO_FILE_GENERATIONS
        
        SRC_IDL :=
        
        include $(COMPONENTS_MK)

        GLOBAL_HEADER_FILES_DEPS := $(patsubst %,%/*.h,$(GLOBAL_INCLUDE_DIR)) 

        #update all variables that use new generated files
        include $(MAKEFILES_ROOT_DIR)/common.mk

        #remove duplicate entries :
        GLOBAL_INCLUDE_DIR := $(sort $(GLOBAL_INCLUDE_DIR))
        SRC_IDL := $(sort $(SRC_IDL))

        SKIP_SECTION_THAT_SHOULD_RUN_ONCE_AFTER_AUTO_FILE_GENERATIONS = YES
        export #export all variables declared before
    endif
endif



all: $(COMPONENTS_MK)
	$(eval MAKEFILE_WAS_GENERATED := YES)
	$(eval export MAKEFILE_WAS_GENERATED)
	$(MAKE) all_after_makefile_generated

all_after_makefile_generated : build_outputs
	$(eval TARGET := all)
	$(info ---- Compiling project $(APP_ROOT_DIR) done at $(TIME_STR) ----)

rebuild : clean
	$(MAKE) all


$(COMPONENTS_MK) : $(APP_ROOT_DIR)/.config
	$(MAKE) -f $(GENERATE_PRJ_FILES_DIR)/generate_include_components_mk_file.mk


build_outputs :  $(SUBDIRS)
	$(MAKE) -f $(MAKEFILES_ROOT_DIR)/qaic/build_final_outputs.mk


clean:
	$(RM) $(OUT_DIR)
	$(RM) $(AUTO_GENERATED_FILES_DIR)


list_var:
	$(MAKE) -f $(MAKEFILES_ROOT_DIR)/list_usful_variables.mk



menuconfig:
	$(MAKE) -f $(MAKEFILES_ROOT_DIR)/menuconfig.mk

.PHONY: default rebuild clean all $(SUBDIRS)

endif# test for valid target
