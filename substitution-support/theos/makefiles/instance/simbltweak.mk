ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

.PHONY: internal-simbltweak-all_ internal-simbltweak-stage_ internal-simbltweak-compile

_THEOS_INTERNAL_LDFLAGS += -dynamiclib

# Bundle Setup
LOCAL_INSTALL_PATH ?= $(strip $($(THEOS_CURRENT_INSTANCE)_INSTALL_PATH))
ifeq ($(LOCAL_INSTALL_PATH),)
	LOCAL_INSTALL_PATH = /Library/Application Support/SIMBL/Plugins
endif

ifeq ($($(THEOS_CURRENT_INSTANCE)_BUNDLE_NAME),)
LOCAL_BUNDLE_NAME = $(THEOS_CURRENT_INSTANCE)
else
LOCAL_BUNDLE_NAME = $($(THEOS_CURRENT_INSTANCE)_BUNDLE_NAME)
endif

_LOCAL_BUNDLE_FULL_NAME = $(LOCAL_BUNDLE_NAME).bundle
_THEOS_SHARED_BUNDLE_BUILD_PATH = $(THEOS_OBJ_DIR)/$(_LOCAL_BUNDLE_FULL_NAME)
_THEOS_SHARED_BUNDLE_STAGE_PATH = $(THEOS_STAGING_DIR)$(LOCAL_INSTALL_PATH)/$(_LOCAL_BUNDLE_FULL_NAME)
_LOCAL_INSTANCE_TARGET := $(_LOCAL_BUNDLE_FULL_NAME)$(_THEOS_TARGET_BUNDLE_BINARY_SUBDIRECTORY)/$(THEOS_CURRENT_INSTANCE)$(TARGET_EXE_EXT)
include $(THEOS_MAKE_PATH)/instance/shared/bundle.mk
# End Bundle Setup

_THEOS_INTERNAL_LOGOSFLAGS += -c generator=internal

ifeq ($(_THEOS_MAKE_PARALLEL_BUILDING), no)
internal-simbltweak-all_:: $(_OBJ_DIR_STAMPS) shared-instance-bundle-all $(THEOS_OBJ_DIR)/$(_LOCAL_INSTANCE_TARGET)
else
internal-simbltweak-all_:: $(_OBJ_DIR_STAMPS) shared-instance-bundle-all
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) \
		internal-simbltweak-compile \
		_THEOS_CURRENT_TYPE=$(_THEOS_CURRENT_TYPE) THEOS_CURRENT_INSTANCE=$(THEOS_CURRENT_INSTANCE) _THEOS_CURRENT_OPERATION=compile \
		THEOS_BUILD_DIR="$(THEOS_BUILD_DIR)" _THEOS_MAKE_PARALLEL=yes

internal-simbltweak-compile: $(THEOS_OBJ_DIR)/$(_LOCAL_INSTANCE_TARGET)
endif

$(eval $(call _THEOS_TEMPLATE_DEFAULT_LINKING_RULE,$(_LOCAL_INSTANCE_TARGET)))

internal-simbltweak-stage_:: shared-instance-bundle-stage

$(eval $(call __mod,instance/simbltweak.mk))
