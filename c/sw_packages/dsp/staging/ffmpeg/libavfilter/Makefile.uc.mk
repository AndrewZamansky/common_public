
ifeq ($(findstring YES,$(CONFIG_INCLUDE_FFMPEG)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

INCLUDE_DIR = ffmpeg_config_include

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  




INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavfilter
INCLUDE_DIR += $(PUBLIC_SW_PACKAGES_DIR)/dsp/ffmpeg

SRC += af_biquads.c
SRC += formats.c
SRC += audio.c
SRC += video.c
SRC += avfilter.c
SRC += avfiltergraph.c
SRC += buffer.c
SRC += buffersrc.c
SRC += buffersink.c

SRC += allfilters.c
SRC += opencl_allkernels.c
SRC +=fifo.c
SRC +=avcodec.c

VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavfilter 


include $(COMMON_CC)
