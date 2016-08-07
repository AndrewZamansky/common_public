
ifeq ($(findstring YES,$(CONFIG_INCLUDE_FFMPEG)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

INCLUDE_DIR = ffmpeg_config_include

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

INCLUDE_DIR +=$(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg
INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavutil

SRC += mem.c
SRC += log.c
SRC += bprint.c
SRC += frame.c
SRC += buffer.c
SRC += dict.c
SRC += avstring.c
SRC += pixdesc.c
SRC += imgutils.c
SRC += samplefmt.c
SRC +=channel_layout.c
SRC +=mathematics.c
SRC +=atomic.c
SRC +=rational.c
SRC +=intmath.c
SRC +=log2_tab.c
SRC +=error.c
SRC +=parseutils.c
SRC +=utils.c
SRC +=eval.c
SRC +=opt.c
SRC +=time.c
SRC +=random_seed.c
SRC +=sha.c

SRC +=fifo.c
SRC +=audio_fifo.c

VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ffmpeg/libavutil 

include $(COMMON_CC)
