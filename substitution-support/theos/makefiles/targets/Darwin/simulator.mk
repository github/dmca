ifeq ($(_THEOS_TARGET_LOADED),)
_THEOS_TARGET_LOADED := 1
THEOS_TARGET_NAME := iphone_simulator

_THEOS_TARGET_PLATFORM_NAME := iphonesimulator
_THEOS_TARGET_PLATFORM_SDK_NAME := iPhoneSimulator
_THEOS_TARGET_PLATFORM_FLAG_NAME := ios-simulator
_THEOS_TARGET_PLATFORM_SWIFT_NAME := apple-ios
_THEOS_TARGET_PLATFORM_IS_SIMULATOR := $(_THEOS_TRUE)

include $(THEOS_MAKE_PATH)/targets/_common/darwin_head.mk

ifeq ($(IPHONE_SIMULATOR_ROOT),)
internal-install::
	$(ERROR_BEGIN)"$(MAKE) install for the simulator requires that you set IPHONE_SIMULATOR_ROOT to the root directory of the simulated OS."$(ERROR_END)
else
internal-install:: stage
	$(ECHO_NOTHING)install.mergeDir "$(THEOS_STAGING_DIR)" "$(IPHONE_SIMULATOR_ROOT)"$(ECHO_END)
endif

# We have to figure out the target version here, as we need it in the calculation of the deployment version.
_TARGET_VERSION_GE_3_2 = $(call __simplify,_TARGET_VERSION_GE_3_2,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 3.2))
_TARGET_VERSION_GE_4_0 = $(call __simplify,_TARGET_VERSION_GE_4_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 4.0))
_TARGET_VERSION_GE_7_0 = $(call __simplify,_TARGET_VERSION_GE_7_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 7.0))
_TARGET_VERSION_GE_8_0 = $(call __simplify,_TARGET_VERSION_GE_8_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 8.0))
_TARGET_VERSION_GE_8_4 = $(call __simplify,_TARGET_VERSION_GE_8_4,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 8.4))
_TARGET_VERSION_GE_11_0 = $(call __simplify,_TARGET_VERSION_GE_11_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 11.0))

ARCHS ?= $(if $(_TARGET_VERSION_GE_8_0),,i386) $(if $(_TARGET_VERSION_GE_7_0),x86_64)
NEUTRAL_ARCH = $(if $(_TARGET_VERSION_GE_8_0),x86_64,i386)

_TARGET_VERSION_FLAG = $(if $(_TARGET_VERSION_GE_7_0),-mios-simulator-version-min=$(_THEOS_TARGET_IPHONEOS_DEPLOYMENT_VERSION),-mmacosx-version-min=$(if $(_TARGET_VERSION_GE_4_0),10.6,10.5))
_TARGET_OBJC_ABI_CFLAGS = $(if $(_TARGET_VERSION_GE_3_2),-fobjc-abi-version=2 -fobjc-legacy-dispatch)
_TARGET_OBJC_ABI_LDFLAGS = $(if $(_TARGET_VERSION_GE_3_2),-Xlinker -objc_abi_version -Xlinker 2)

ifeq ($(_TARGET_VERSION_GE_8_4),1)
_THEOS_DARWIN_CAN_USE_MODULES := 1
endif

include $(THEOS_MAKE_PATH)/targets/_common/darwin_tail.mk

ifeq ($(_TARGET_VERSION_GE_7_0),)
	VERSIONFLAGS = -mmacosx-version-min=$(if $(_TARGET_VERSION_GE_4_0),10.6,10.5)
endif

_TARGET_OBJC_ABI_CFLAGS := $(if $(_TARGET_VERSION_GE_3_2),-fobjc-abi-version=2 -fobjc-legacy-dispatch)
_TARGET_OBJC_ABI_LDFLAGS := $(if $(_TARGET_VERSION_GE_3_2),-Xlinker -objc_abi_version -Xlinker 2)

_THEOS_TARGET_CFLAGS += $(_TARGET_OBJC_ABI_CFLAGS)
_THEOS_TARGET_LDFLAGS += $(_TARGET_OBJC_ABI_LDFLAGS) -Xlinker -allow_simulator_linking_to_macosx_dylibs
endif
