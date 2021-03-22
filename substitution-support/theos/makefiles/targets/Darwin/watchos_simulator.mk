ifeq ($(_THEOS_TARGET_LOADED),)
_THEOS_TARGET_LOADED := 1
THEOS_TARGET_NAME := watchos_simulator

_THEOS_TARGET_PLATFORM_NAME := watchsimulator
_THEOS_TARGET_PLATFORM_SDK_NAME := WatchSimulator
_THEOS_TARGET_PLATFORM_FLAG_NAME := watchos-simulator
_THEOS_TARGET_PLATFORM_SWIFT_NAME := apple-watchos
_THEOS_TARGET_PLATFORM_IS_SIMULATOR := $(_THEOS_TRUE)
_THEOS_DARWIN_CAN_USE_MODULES := $(_THEOS_TRUE)

NEUTRAL_ARCH := i386

include $(THEOS_MAKE_PATH)/targets/_common/darwin_head.mk
include $(THEOS_MAKE_PATH)/targets/_common/darwin_tail.mk

ifeq ($(WATCHOS_SIMULATOR_ROOT),)
internal-install::
	$(ERROR_BEGIN)"$(MAKE) install for the simulator requires that you set WATCHOS_SIMULATOR_ROOT to the root directory of the simulated OS."$(ERROR_END)
else
internal-install:: stage
	$(ECHO_NOTHING)install.mergeDir "$(THEOS_STAGING_DIR)" "$(WATCHOS_SIMULATOR_ROOT)"$(ECHO_END)
endif

_TARGET_OBJC_ABI_CFLAGS = -fobjc-abi-version=2 -fobjc-legacy-dispatch
_TARGET_OBJC_ABI_LDFLAGS = -Xlinker -objc_abi_version -Xlinker 2 -Xlinker -allow_simulator_linking_to_macosx_dylibs

_THEOS_TARGET_CFLAGS := -isysroot $(ISYSROOT) $(SDKFLAGS) $(_TARGET_OBJC_ABI_CFLAGS) $(MODULESFLAGS)
_THEOS_TARGET_LDFLAGS := -isysroot $(SYSROOT) $(SDKFLAGS) -multiply_defined suppress $(_TARGET_OBJC_ABI_LDFLAGS)
endif
