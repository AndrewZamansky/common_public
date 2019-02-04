INCLUDE_THIS_COMPONENT :=$(CONFIG_INCLUDE_USB_AUDIO_CLASS)


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC += usb_audio_class.c
SRC += usb_audio_class_descriptors.c
VPATH = src



include $(COMMON_CC)
