ifdef CONFIG_XTENSA_SIM
    #INCLUDE_THIS_COMPONENT := y
    INCLUDE_THIS_FOR_H_FILES_PATH :=y

    POST_BUILD_MAKEFILE_DIR :=$(COMPONENT_PATH)
    
endif

include $(COMMON_CC)
