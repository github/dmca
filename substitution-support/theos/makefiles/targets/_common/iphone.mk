# We have to figure out the target version here, as we need it in the calculation of the deployment version.
_TARGET_VERSION_GE_12_1 = $(call __simplify,_TARGET_VERSION_GE_12_1,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 12.1))
_TARGET_VERSION_GE_12_0 = $(call __simplify,_TARGET_VERSION_GE_12_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 12.0))
_TARGET_VERSION_GE_10_0 = $(call __simplify,_TARGET_VERSION_GE_10_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 10.0))
_TARGET_VERSION_GE_8_4 = $(call __simplify,_TARGET_VERSION_GE_8_4,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 8.4))
_TARGET_VERSION_GE_7_0 = $(call __simplify,_TARGET_VERSION_GE_7_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 7.0))
_TARGET_VERSION_GE_6_0 = $(call __simplify,_TARGET_VERSION_GE_6_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 6.0))
_TARGET_VERSION_GE_4_0 = $(call __simplify,_TARGET_VERSION_GE_4_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 4.0))
_TARGET_VERSION_GE_3_0 = $(call __simplify,_TARGET_VERSION_GE_3_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_SDK_VERSION) ge 3.0))

ifeq ($(_TARGET_VERSION_GE_12_0),1)
	_TARGET_LIBCPP_CCFLAGS := -stdlib=libc++
	_TARGET_LIBCPP_LDFLAGS := -stdlib=libc++ -lc++
endif

ifeq ($(_TARGET_VERSION_GE_10_0),1)
	_THEOS_TARGET_DEFAULT_OS_DEPLOYMENT_VERSION := 6.0
else ifeq ($(_TARGET_VERSION_GE_7_0),1)
	_THEOS_TARGET_DEFAULT_OS_DEPLOYMENT_VERSION := 5.0
else ifeq ($(_TARGET_VERSION_GE_6_0),1)
	_THEOS_TARGET_DEFAULT_OS_DEPLOYMENT_VERSION := 4.3
else
	_THEOS_TARGET_DEFAULT_OS_DEPLOYMENT_VERSION := 3.0
endif

_DEPLOY_VERSION_GE_11_0 = $(call __simplify,_DEPLOY_VERSION_GE_11_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_OS_DEPLOYMENT_VERSION) ge 11.0))
_DEPLOY_VERSION_GE_5_0 = $(call __simplify,_DEPLOY_VERSION_GE_5_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_OS_DEPLOYMENT_VERSION) ge 5.0))
_DEPLOY_VERSION_GE_3_0 = $(call __simplify,_DEPLOY_VERSION_GE_3_0,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_OS_DEPLOYMENT_VERSION) ge 3.0))
_DEPLOY_VERSION_LT_4_3 = $(call __simplify,_DEPLOY_VERSION_LT_4_3,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(_THEOS_TARGET_OS_DEPLOYMENT_VERSION) lt 4.3))

ifeq ($(_TARGET_VERSION_GE_6_0)$(_DEPLOY_VERSION_GE_3_0)$(_DEPLOY_VERSION_LT_4_3),111)
ifeq ($(ARCHS)$(IPHONE_ARCHS)$(_THEOS_TARGET_WARNED_DEPLOY),)
before-all::
	@$(PRINT_FORMAT_WARNING) "Deploying to iOS 3.0 while building for 6.0 will generate armv7-only binaries." >&2
export _THEOS_TARGET_WARNED_DEPLOY := 1
endif
endif

ifeq ($(_DEPLOY_VERSION_GE_11_0),1) # } Deploy >= 11.0 {
ifeq ($(_TARGET_VERSION_GE_12_1)$(THEOS_PLATFORM_NAME),1macosx) # >= 12.1, macOS {
	ARCHS ?= arm64 arm64e
else # } else {
	ARCHS ?= arm64
endif # }
else ifeq ($(_TARGET_VERSION_GE_7_0),1) # } >= 7.0 {
ifeq ($(_TARGET_VERSION_GE_12_1)$(THEOS_PLATFORM_NAME),1macosx) # >= 12.1, macOS {
	ARCHS ?= armv7 arm64 arm64e
else # } else {
	ARCHS ?= armv7 arm64
endif # }
else ifeq ($(_TARGET_VERSION_GE_6_0),1) # } >= 6.0 {
ifeq ($(_TARGET_VERSION_GE_7_0)$(_DEPLOY_VERSION_GE_5_0),11) # >= 7.0, Deploy >= 5.0 {
	ARCHS ?= armv7 arm64
else # } else {
	ARCHS ?= armv7
endif # }
else ifeq ($(_TARGET_VERSION_GE_3_0),1) # } >= 3.0 {
	ARCHS ?= armv6 armv7
else # } < 3.0 {
	ARCHS ?= armv6
endif # }

ifeq ($(_TARGET_VERSION_GE_11_0),1)
	NEUTRAL_ARCH := arm64
else
	NEUTRAL_ARCH := armv7
endif

ifeq ($(_TARGET_VERSION_GE_8_4),1)
	_THEOS_DARWIN_CAN_USE_MODULES := $(_THEOS_TRUE)
endif
