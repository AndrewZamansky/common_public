
ifdef CONFIG_APQ8096

    APQ8096_SDK_VERSION :=3.2
    __TMP_HEXAGON_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/Hexagon_SDK
    SDK_APQ8096_PATH :=$(__TMP_HEXAGON_PATH)/$(APQ8096_SDK_VERSION)
    ifeq ("$(wildcard $(SDK_APQ8096_PATH))","")
        $(info !--- APQ8096 BSP path $(SDK_APQ8096_PATH) dont exists )
        $(info !--- download APQ8096 SDK version $(APQ8096_SDK_VERSION) and unpack it to $(SDK_APQ8096_PATH)  )
        $(info !--- make sure incs and libs  folders is located in $(SDK_APQ8096_PATH)/  after unpacking   )
        $(error )
    endif
    
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_APQ8096_PATH)/incs)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_APQ8096_PATH)/incs/audio)
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(SDK_APQ8096_PATH)/incs/stddef)
	APQ8096_SRC_DIR := $(SDK_APQ8096_PATH)/libs
	APQ8096_SIM_SRC_DIR := $(SDK_APQ8096_PATH)/test
	#INCLUDE_THIS_COMPONENT := y
	#INCLUDE_THIS_FOR_H_FILES_PATH :=y
	
endif

include $(COMMON_CC)
