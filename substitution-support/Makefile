THEOS ?= theos
TARGET ?= iphone:latest:12.1
GO_EASY_ON_ME = 1
TARGET_CODESIGN = $(abspath ./ldid)
SIGNCERT = $(abspath ./layout/usr/share/substitute/signcert.p12)

include $(THEOS)/makefiles/common.mk

ifeq ($(call __theos_bool,$(or $(debug),$(DEBUG))),$(_THEOS_TRUE))
	PACKAGE_BUILDNAME="$(shell git show-ref --abbrev HEAD --hash)+debug"
endif


TARGET_CC = clang
TARGET_CXX = clang++

ADDITIONAL_CFLAGS = -O2
ADDITIONAL_CFLAGS += -Wno-nullability-completeness
ADDITIONAL_CCFLAGS = -std=c++17

_THEOS_TARGET_ONLY_OBJCFLAGS = -std=gnu11

TOOL_NAME = substituted substitute-inject substitute-reload
LIBRARY_NAME = substitute-inserter substitute-injector

offset_cache_FILES = offset-cache/offsetcache.c
offset_cache_CFLAGS = -Ioffset-cache
stringhash/stringarray.c_CFLAGS = -Wno-unused-variable
stringhash/stringhash.c_CFLAGS = -Wno-unused-variable

substituted_ARCHS = arm64
substituted_FILES = substituted.cc amfidServer.c substitute_daemonServer.c $(offset_cache_FILES) stringhash/stringarray.c stringhash/stringhash.c libforgery/libforgery.cc cs_bypass.cc
substituted_LDFLAGS = -Xlinker -unexported_symbol -Xlinker "*" libs/libsubstitute.a
substituted_CFLAGS = -DTHEOS_LEAN_AND_MEAN -DLAUNCH_PROXYLESS -fobjc-arc -fno-rtti -Iinclude -Wall $(offset_cache_CFLAGS) -I/usr/local/opt/openssl/ -Ilibforgery -Istringhash
substituted_FRAMEWORKS = CoreFoundation Security
substituted_CODESIGN_FLAGS = -K$(SIGNCERT) -Ssubstituted.entitlements
substituted_INSTALL_PATH = /usr/libexec

substitute-inserter_ARCHS = arm64 arm64e
substitute-inserter_FILES = substitute-inserter.cc substitute_daemonUser.c
substitute-inserter_LDFLAGS = -Xlinker -no_warn_inits -Xlinker -unexported_symbol -Xlinker "*" libs/libsubstitute.a
substitute-inserter_CFLAGS = -fobjc-arc -fno-rtti -Wall -Iinclude -Ilibforgery -Istringhash -Wno-format-security
substitute-inserter_CODESIGN_FLAGS = -K$(SIGNCERT) -S
substitute-inserter_INSTALL_PATH = /usr/lib

substitute-injector_ARCHS = arm64 arm64e
substitute-injector_FILES = substitute-injector.c
substitute-injector_LDFLAGS = -Xlinker -unexported_symbol -Xlinker "*"
substitute-injector_CFLAGS = -Iinclude -Wall
substitute-injector_FRAMEWORKS = CoreFoundation
substitute-injector_CODESIGN_FLAGS = -K$(SIGNCERT) -S
substitute-injector_INSTALL_PATH = /usr/lib

substitute-inject_ARCHS = arm64 arm64e
substitute-inject_FILES = substitute-inject.c substitute-injector.c
substitute-inject_LDFLAGS = -Xlinker -unexported_symbol -Xlinker "*"
substitute-inject_CFLAGS = -Iinclude -Wall
substitute-inject_FRAMEWORKS = CoreFoundation
substitute-inject_CODESIGN_FLAGS = -K$(SIGNCERT) -Ssubstitute-inject.entitlements
substitute-inject_INSTALL_PATH = /usr/bin

substitute-reload_ARCHS = arm64
substitute-reload_FILES = substitute-reload.c
substitute-reload_LDFLAGS = -Xlinker -unexported_symbol -Xlinker "*"
substitute-reload_CFLAGS = -Iinclude -Wall
substitute-reload_FRAMEWORKS = CoreFoundation
substitute-reload_CODESIGN_FLAGS = -K$(SIGNCERT) -Ssubstitute-inject.entitlements
substitute-reload_INSTALL_PATH = /usr/libexec

stage::
	$(ECHO_NOTHING)cd $(THEOS_STAGING_DIR); \
	find $(THEOS_STAGING_DIR) \( -path $(THEOS_STAGING_DIR)/DEBIAN -o -path $(THEOS_STAGING_DIR)/usr/share/substitute/resources.txt \) -prune -o -type f -print0 | xargs -0 sha1sum > $(THEOS_STAGING_DIR)/usr/share/substitute/resources.txt$(ECHO_END)

include $(THEOS_MAKE_PATH)/library.mk
include $(THEOS_MAKE_PATH)/tool.mk
