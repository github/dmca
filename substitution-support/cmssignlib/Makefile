include $(THEOS)/makefiles/common.mk

LIBRARY_NAME = cmssignlib
cmssignlib_FILES = cmssignlib.c libplist/libcnary/node.c libplist/libcnary/node_list.c $(wildcard libplist/src/*.c)

export ARCHS = arm64
CFLAGS = -Ilibplist/include -Ilibplist/libcnary/include -Ilibplist/src/ -I/usr/local/opt/openssl/include -Wno-unused-variable -O0
LDFLAGS = -L/usr/local/opt/openssl/lib -lcryptoARM64

include $(THEOS_MAKE_PATH)/library.mk
