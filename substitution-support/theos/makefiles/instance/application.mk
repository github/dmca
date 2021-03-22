ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

.PHONY: internal-application-all_ internal-application-stage_ internal-application-compile

ifeq ($(MODULESFLAGS),)
ifeq ($(findstring UIKit,$($(THEOS_CURRENT_INSTANCE)_FRAMEWORKS))$(findstring AppKit,$($(THEOS_CURRENT_INSTANCE)_FRAMEWORKS)),)
_THEOS_INTERNAL_LDFLAGS += -framework UIKit
ifeq ($(_THEOS_APPLICATION_WARNED_IMPLICIT_UIKIT_$(THEOS_CURRENT_INSTANCE)),)
internal-application-all_::
	@$(PRINT_FORMAT_WARNING) "$(THEOS_CURRENT_INSTANCE): warning: Implicit UIKit linkage for application instances is deprecated. Please add \"UIKit\" to $(THEOS_CURRENT_INSTANCE)_FRAMEWORKS." >&2
export _THEOS_APPLICATION_WARNED_IMPLICIT_UIKIT_$(THEOS_CURRENT_INSTANCE) = 1
endif
endif
endif

# Bundle Setup
LOCAL_INSTALL_PATH ?= $(strip $($(THEOS_CURRENT_INSTANCE)_INSTALL_PATH))
ifeq ($(LOCAL_INSTALL_PATH),)
	LOCAL_INSTALL_PATH = /Applications
endif

ifeq ($($(THEOS_CURRENT_INSTANCE)_BUNDLE_NAME),)
LOCAL_BUNDLE_NAME = $(THEOS_CURRENT_INSTANCE)
else
LOCAL_BUNDLE_NAME = $($(THEOS_CURRENT_INSTANCE)_BUNDLE_NAME)
endif

_LOCAL_BUNDLE_FULL_NAME = $(LOCAL_BUNDLE_NAME).app
_THEOS_SHARED_BUNDLE_BUILD_PATH = $(THEOS_OBJ_DIR)/$(_LOCAL_BUNDLE_FULL_NAME)
_THEOS_SHARED_BUNDLE_STAGE_PATH = $(THEOS_STAGING_DIR)$(LOCAL_INSTALL_PATH)/$(_LOCAL_BUNDLE_FULL_NAME)
_LOCAL_INSTANCE_TARGET := $(_LOCAL_BUNDLE_FULL_NAME)$(_THEOS_TARGET_BUNDLE_BINARY_SUBDIRECTORY)/$(THEOS_CURRENT_INSTANCE)$(TARGET_EXE_EXT)
include $(THEOS_MAKE_PATH)/instance/shared/bundle.mk
# End Bundle Setup

ifeq ($(_THEOS_MAKE_PARALLEL_BUILDING), no)
internal-application-all_:: $(_OBJ_DIR_STAMPS) shared-instance-bundle-all $(THEOS_OBJ_DIR)/$(_LOCAL_INSTANCE_TARGET)
else
internal-application-all_:: $(_OBJ_DIR_STAMPS) shared-instance-bundle-all
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) \
		internal-application-compile \
		_THEOS_CURRENT_TYPE=$(_THEOS_CURRENT_TYPE) THEOS_CURRENT_INSTANCE=$(THEOS_CURRENT_INSTANCE) _THEOS_CURRENT_OPERATION=compile \
		THEOS_BUILD_DIR="$(THEOS_BUILD_DIR)" _THEOS_MAKE_PARALLEL=yes

internal-application-compile: $(THEOS_OBJ_DIR)/$(_LOCAL_INSTANCE_TARGET)
endif

$(eval $(call _THEOS_TEMPLATE_DEFAULT_LINKING_RULE,$(_LOCAL_INSTANCE_TARGET)))

internal-application-stage_:: shared-instance-bundle-stage

$(eval $(call __mod,instance/application.mk))
