ifeq ($(sort $(CONFIG_XTENSA_SIM)),y)
    INCLUDE_THIS_COMPONENT := y #need to be set to add flags for linker
    #INCLUDE_THIS_FOR_H_FILES_PATH :=y

    # !!IMPORTANT!! : should be used before any 'include' statement in makefile:
    CURR_COMPONENT_DIR :=$(call get_curr_component_dir)

    $(eval $(call ADD_TO_GLOBAL_LDFLAGS, -mlsp=sim))

    # post build routines are set in application postbuild makefile
    # DUMMY :=$(call SET_DIR_WITH_POSTBUILD_MAKEFILE, $(CURR_COMPONENT_DIR))

endif

include $(COMMON_CC)
