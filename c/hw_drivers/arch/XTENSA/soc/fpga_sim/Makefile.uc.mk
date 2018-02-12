ifeq ($(sort $(CONFIG_XTENSA_FPGA_SIM)),y)
    #INCLUDE_THIS_COMPONENT := y
    INCLUDE_THIS_FOR_H_FILES_PATH :=y
    
endif

include $(COMMON_CC)
