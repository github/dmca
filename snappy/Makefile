target ?= iphone:clang:11.0:10.0
ARCHS ?= arm64 armv7
DEBUG ?= no
include $(THEOS)/makefiles/common.mk

LIBRARY_NAME = libsnappy
libsnappy_FILES = libsnappy.c
libsnappy_FRAMEWORKS = IOKit

TOOL_NAME = snappy
snappy_FILES = snappy.m
snappy_LDFLAGS = -L$(THEOS_OBJ_DIR) -lsnappy
snappy_CODESIGN_FLAGS = -Sentitlements.xml

after-stage::
	$(ECHO_NOTHING)chmod u+s $(THEOS_STAGING_DIR)/usr/bin/snappy$(ECHO_END)

include $(THEOS_MAKE_PATH)/library.mk
include $(THEOS_MAKE_PATH)/tool.mk
