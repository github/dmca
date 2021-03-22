ifeq ($(_THEOS_TARGET_LOADED),)
_THEOS_TARGET_LOADED := 1
THEOS_TARGET_NAME := iphone

_THEOS_TARGET_PLATFORM_NAME := iphoneos
_THEOS_TARGET_PLATFORM_SDK_NAME := iPhoneOS
_THEOS_TARGET_PLATFORM_FLAG_NAME := iphoneos
_THEOS_TARGET_PLATFORM_SWIFT_NAME := apple-ios

include $(THEOS_MAKE_PATH)/targets/_common/darwin_head.mk
include $(THEOS_MAKE_PATH)/targets/_common/iphone.mk
include $(THEOS_MAKE_PATH)/targets/_common/darwin_tail.mk
endif
