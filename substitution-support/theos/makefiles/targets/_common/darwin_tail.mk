# if the project defines no value for ARCHS, and the target makefile hasn’t already set a default
# ARCHS, set it to the value of NEUTRAL_ARCH
ARCHS ?= $(NEUTRAL_ARCH)

# determine the SYSROOT (for linking) and ISYSROOT (for compiling, I for “include”) like so:
# SYSROOT: check for theos/sdks/BlahX.Y.sdk, or Xcode.app/…/SDKs/BlahX.Y.sdk, or leave empty so we
# show an error
# ISYSROOT: check for theos/sdks/BlahA.B.sdk, or Xcode.app/…/SDKs/BlahA.B.sdk, or use the same value
# as SYSROOT if not defined
SYSROOT ?= $(or $(wildcard $(THEOS_SDKS_PATH)/$(_THEOS_TARGET_PLATFORM_SDK_NAME)$(_THEOS_TARGET_SDK_VERSION).sdk),$(wildcard $(THEOS_PLATFORM_SDK_ROOT)/Platforms/$(_THEOS_TARGET_PLATFORM_SDK_NAME).platform/Developer/SDKs/$(_THEOS_TARGET_PLATFORM_SDK_NAME)$(_THEOS_TARGET_SDK_VERSION).sdk))
ISYSROOT ?= $(or $(wildcard $(THEOS_SDKS_PATH)/$(_THEOS_TARGET_PLATFORM_SDK_NAME)$(_THEOS_TARGET_INCLUDE_SDK_VERSION).sdk),$(wildcard $(THEOS_PLATFORM_SDK_ROOT)/Platforms/$(_THEOS_TARGET_PLATFORM_SDK_NAME).platform/Developer/SDKs/$(_THEOS_TARGET_PLATFORM_SDK_NAME)$(_THEOS_TARGET_INCLUDE_SDK_VERSION).sdk),$(SYSROOT))

TARGET_PRIVATE_FRAMEWORK_PATH ?= $(SYSROOT)/System/Library/PrivateFrameworks
TARGET_PRIVATE_FRAMEWORK_INCLUDE_PATH ?= $(ISYSROOT)/System/Library/PrivateFrameworks

# if the toolchain is capable of using clang modules, define the flags that enable modules
ifeq ($(_THEOS_DARWIN_CAN_USE_MODULES),1)
	MODULESFLAGS := -fmodules -fcxx-modules -fmodule-name=$(THEOS_CURRENT_INSTANCE) -fbuild-session-file=$(_THEOS_BUILD_SESSION_FILE) \
		-fmodules-prune-after=345600 -fmodules-prune-interval=86400 -fmodules-validate-once-per-build-session
endif

VERSIONFLAGS := -m$(_THEOS_TARGET_PLATFORM_FLAG_NAME)-version-min=$(_THEOS_TARGET_OS_DEPLOYMENT_VERSION)

# “iOS 9 changed the 32-bit pagesize on 64-bit CPUs from 4096 bytes to 16384: all 32-bit binaries
# must now be compiled with -Wl,-segalign,4000.” https://twitter.com/saurik/status/654198997024796672
ifneq ($(THEOS_CURRENT_ARCH),arm64)
	LEGACYFLAGS := -Xlinker -segalign -Xlinker 4000
endif

_THEOS_TARGET_CFLAGS := -isysroot "$(ISYSROOT)" $(VERSIONFLAGS) $(MODULESFLAGS) $(_THEOS_TARGET_CC_CFLAGS)
_THEOS_TARGET_CCFLAGS := $(_TARGET_LIBCPP_CCFLAGS)
_THEOS_TARGET_LDFLAGS := -isysroot "$(SYSROOT)" $(VERSIONFLAGS) $(LEGACYFLAGS) -multiply_defined suppress $(_TARGET_LIBCPP_LDFLAGS)

_THEOS_TARGET_SWIFTFLAGS := -sdk "$(SYSROOT)" $(_THEOS_TARGET_CC_SWIFTFLAGS)
_THEOS_TARGET_SWIFT_TARGET := $(_THEOS_TARGET_PLATFORM_SWIFT_NAME)$(_THEOS_TARGET_OS_DEPLOYMENT_VERSION)

ifeq ($(call __executable,$(TARGET_SWIFT)),$(_THEOS_TRUE))
	_THEOS_TARGET_SWIFT_VERSION = $(call __simplify,_THEOS_TARGET_SWIFT_VERSION,$(shell $(TARGET_SWIFT) --version | head -1 | cut -d'v' -f2 | cut -d' ' -f2 | cut -d'-' -f1))
ifeq ($(firstword $(subst ., ,$(_THEOS_TARGET_SWIFT_VERSION))),4)
	_THEOS_TARGET_SWIFT_VERSION_PATH = $(_THEOS_TARGET_SWIFT_VERSION)
else
	_THEOS_TARGET_SWIFT_VERSION_PATH = stable
endif
	_THEOS_TARGET_SWIFT_LDFLAGS := $(call __simplify,_THEOS_TARGET_SWIFT_LDFLAGS,-rpath /usr/lib/libswift/$(_THEOS_TARGET_SWIFT_VERSION_PATH))
	_THEOS_TARGET_SWIFT_LDPATH = $(call __simplify,_THEOS_TARGET_SWIFT_LDPATH,$(dir $(shell type -p $(TARGET_SWIFT)))../lib/swift/$(_THEOS_TARGET_PLATFORM_NAME))
endif

ifeq ($(_THEOS_TARGET_DARWIN_BUNDLE_TYPE),hierarchial)
	_THEOS_TARGET_BUNDLE_INFO_PLIST_SUBDIRECTORY := /Contents
	_THEOS_TARGET_BUNDLE_RESOURCE_SUBDIRECTORY := /Contents/Resources
	_THEOS_TARGET_BUNDLE_BINARY_SUBDIRECTORY := /Contents/MacOS
	_THEOS_TARGET_BUNDLE_HEADERS_SUBDIRECTORY := /Contents/Headers
else
	_THEOS_TARGET_BUNDLE_INFO_PLIST_SUBDIRECTORY :=
	_THEOS_TARGET_BUNDLE_RESOURCE_SUBDIRECTORY :=
	_THEOS_TARGET_BUNDLE_BINARY_SUBDIRECTORY :=
	_THEOS_TARGET_BUNDLE_HEADERS_SUBDIRECTORY := /Headers
endif
