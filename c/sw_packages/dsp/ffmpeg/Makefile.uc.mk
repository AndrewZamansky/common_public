
ifeq ($(findstring YES,$(_INCLUDE_DSP_MANAGEMENT)),YES) 	 
	INCLUDE_THIS_FOR_H_FILES_PATH := YES
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg )
    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavfilter )
endif  

