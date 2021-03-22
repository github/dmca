ifeq ($(_THEOS_TARGET_LOADED),)
_THEOS_TARGET_LOADED := 1
THEOS_TARGET_NAME := watchos

_THEOS_TARGET_PLATFORM_NAME := watchos
_THEOS_TARGET_PLATFORM_SDK_NAME := WatchOS
_THEOS_TARGET_PLATFORM_FLAG_NAME := watchos
_THEOS_TARGET_PLATFORM_SWIFT_NAME := apple-watchos
_THEOS_DARWIN_CAN_USE_MODULES := $(_THEOS_TRUE)

NEUTRAL_ARCH := armv7k

include $(THEOS_MAKE_PATH)/targets/_common/darwin_head.mk
include $(THEOS_MAKE_PATH)/targets/_common/darwin_tail.mk
endif
