
ifeq ($(findstring YES,$(CONFIG_INCLUDE_UGFX)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
	GLOBAL_INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx
	GLOBAL_INCLUDE_DIR += $(SW_PACKAGES_ROOT_DIR)/ugfx/include
endif  

#INCLUDE_DIR =   

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

SRC=


INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/drivers/gdisp/SSD1963
SRC += gdisp_lld_SSD1963.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/drivers/gdisp/SSD1963


SRC += gfx.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/src

INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/src/gdisp
SRC += gdisp_mk.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/src/gdisp

INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/src/gdisp
SRC += gos_mk.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/src/gos

INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/src/gdriver
SRC += gdriver_mk.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ugfx/src/gdriver


include $(COMMON_CC)
