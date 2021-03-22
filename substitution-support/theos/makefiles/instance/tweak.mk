ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

.PHONY: internal-tweak-all_ internal-tweak-stage_

LOCAL_INSTALL_PATH ?= $(strip $($(THEOS_CURRENT_INSTANCE)_INSTALL_PATH))
ifeq ($(LOCAL_INSTALL_PATH),)
	LOCAL_INSTALL_PATH = /Library/MobileSubstrate/DynamicLibraries
endif

_LOCAL_LOGOS_DEFAULT_GENERATOR = $(or $($(THEOS_CURRENT_INSTANCE)_LOGOS_DEFAULT_GENERATOR),$(LOGOS_DEFAULT_GENERATOR),$(_THEOS_TARGET_LOGOS_DEFAULT_GENERATOR),MobileSubstrate)
_THEOS_INTERNAL_LOGOSFLAGS += -c generator=$(_LOCAL_LOGOS_DEFAULT_GENERATOR)

ifeq ($(_LOCAL_LOGOS_DEFAULT_GENERATOR),MobileSubstrate)
_THEOS_INTERNAL_LDFLAGS += -F$(THEOS_VENDOR_LIBRARY_PATH) -framework CydiaSubstrate
endif

include $(THEOS_MAKE_PATH)/instance/library.mk

internal-tweak-all_:: internal-library-all_

internal-tweak-compile: internal-library-compile

ifneq ($(strip $($(THEOS_CURRENT_INSTANCE)_BUNDLE_RESOURCE_DIRS) $($(THEOS_CURRENT_INSTANCE)_BUNDLE_RESOURCE_FILES)),)
_LOCAL_BUNDLE_INSTALL_PATH = $(or $($(THEOS_CURRENT_INSTANCE)_BUNDLE_INSTALL_PATH),/Library/Application Support/$(THEOS_CURRENT_INSTANCE))
_LOCAL_BUNDLE_NAME = $(or $($(THEOS_CURRENT_INSTANCE)_BUNDLE_NAME),$(THEOS_CURRENT_INSTANCE))
_LOCAL_BUNDLE_EXTENSION = $(or $($(THEOS_CURRENT_INSTANCE)_BUNDLE_EXTENSION),bundle)

_THEOS_SHARED_BUNDLE_BUILD_PATH = $(THEOS_OBJ_DIR)/$(_LOCAL_BUNDLE_NAME).$(_LOCAL_BUNDLE_EXTENSION)
_THEOS_SHARED_BUNDLE_STAGE_PATH = $(THEOS_STAGING_DIR)$(_LOCAL_BUNDLE_INSTALL_PATH)/$(_LOCAL_BUNDLE_NAME).$(_LOCAL_BUNDLE_EXTENSION)
include $(THEOS_MAKE_PATH)/instance/shared/bundle.mk

internal-tweak-all_:: shared-instance-bundle-all
internal-tweak-stage_:: shared-instance-bundle-stage
endif

ifneq ($($(THEOS_CURRENT_INSTANCE)_INSTALL),0)
internal-tweak-stage_:: $(_EXTRA_TARGET) internal-library-stage_
ifeq ($(call __exists,$(THEOS_CURRENT_INSTANCE).plist),$(_THEOS_TRUE))
	$(ECHO_NOTHING)cp $(THEOS_CURRENT_INSTANCE).plist "$(THEOS_STAGING_DIR)$(LOCAL_INSTALL_PATH)/"$(ECHO_END)
else ifeq ($(call __exists,$(THEOS_LAYOUT_DIR_NAME)/$(LOCAL_INSTALL_PATH)/$(THEOS_CURRENT_INSTANCE).plist),$(_THEOS_FALSE))
	$(ERROR_BEGIN)"You are missing a filter property list. Make sure it’s named $(THEOS_CURRENT_INSTANCE).plist. Refer to http://iphonedevwiki.net/index.php/Cydia_Substrate#MobileLoader."$(ERROR_END)
endif
endif

$(eval $(call __mod,instance/tweak.mk))
