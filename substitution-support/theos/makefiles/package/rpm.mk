ifeq ($(_THEOS_PACKAGE_FORMAT_LOADED),)
_THEOS_PACKAGE_FORMAT_LOADED := 1

_THEOS_RPM_PACKAGE_SPEC_PATH := $(wildcard $(THEOS_PROJECT_DIR)/package.spec)
_THEOS_RPM_CAN_PACKAGE := $(if $(_THEOS_RPM_PACKAGE_SPEC_PATH),$(_THEOS_TRUE),$(_THEOS_FALSE))
_THEOS_PACKAGE_INC_VERSION_PREFIX :=
_THEOS_PACKAGE_EXTRA_VERSION_PREFIX := +

_THEOS_RPM_HAS_RPMBUILD := $(call __executable,rpmbuild)
ifneq ($(_THEOS_RPM_HAS_RPMBUILD),$(_THEOS_TRUE))
internal-package-check::
	$(ERROR_BEGIN)"$(MAKE) package requires rpmbuild."$(ERROR_END)
endif

ifeq ($(_THEOS_RPM_CAN_PACKAGE),$(_THEOS_TRUE)) # Control file found (or layout directory found.)
THEOS_PACKAGE_NAME := $(shell grep -i "^Name:" "$(_THEOS_RPM_PACKAGE_SPEC_PATH)" | cut -d':' -f2- | xargs)
THEOS_PACKAGE_ARCH := $(shell grep -i "^BuildArch:" "$(_THEOS_RPM_PACKAGE_SPEC_PATH)" | cut -d':' -f2- | xargs)
ifeq ($(_THEOS_FINAL_PACKAGE),$(_THEOS_TRUE))
THEOS_PACKAGE_BASE_VERSION := $(shell grep -i "^Version:" "$(_THEOS_RPM_PACKAGE_SPEC_PATH)" | cut -d':' -f2- | xargs)-$(shell grep -i "^Release:" "$(_THEOS_RPM_PACKAGE_SPEC_PATH)" | cut -d':' -f2- | xargs)
else
THEOS_PACKAGE_BASE_VERSION := $(shell grep -i "^Version:" "$(_THEOS_RPM_PACKAGE_SPEC_PATH)" | cut -d':' -f2- | xargs)
endif

before-package::
ifeq ($(_THEOS_FINAL_PACKAGE),$(_THEOS_TRUE))
	$(ECHO_NOTHING)sed -e '/^[Ss]ource0:/d' "$(_THEOS_RPM_PACKAGE_SPEC_PATH)" > "$(THEOS_OBJ_DIR)/package.spec"$(ECHO_END)
else
	$(ECHO_NOTHING)sed -e 's/[Rr]elease:.*/Release:	'$(shell echo "$(_THEOS_INTERNAL_PACKAGE_VERSION)" | cut -d - -f 2-)'/g' -e '/^[Ss]ource0:/d' "$(_THEOS_RPM_PACKAGE_SPEC_PATH)" > "$(THEOS_OBJ_DIR)/package.spec"$(ECHO_END)
endif
	$(ECHO_NOTHING)echo '%files' >> "$(THEOS_OBJ_DIR)/package.spec"$(ECHO_END)
	$(ECHO_NOTHING)echo '/*' >> "$(THEOS_OBJ_DIR)/package.spec"$(ECHO_END)

_THEOS_RPM_PACKAGE_FILENAME = $(THEOS_PACKAGE_DIR)/$(THEOS_PACKAGE_NAME)-$(_THEOS_INTERNAL_PACKAGE_VERSION).$(THEOS_PACKAGE_ARCH).rpm
internal-package::
	$(ECHO_NOTHING)COPYFILE_DISABLE=1 $(FAKEROOT) -r rpmbuild -bb "$(THEOS_OBJ_DIR)/package.spec" --buildroot "$(THEOS_STAGING_DIR)" --define "_rpmdir$(_THEOS_RELATIVE_DATA_DIR)/$(THEOS_OBJ_DIR_NAME)" $(STDERR_NULL_REDIRECT)$(STDOUT_NULL_REDIRECT)$(ECHO_END)
	$(ECHO_NOTHING)ln -f "$(THEOS_OBJ_DIR)/$(THEOS_PACKAGE_ARCH)/$(THEOS_PACKAGE_NAME)-$(_THEOS_INTERNAL_PACKAGE_VERSION).$(THEOS_PACKAGE_ARCH).rpm" "$(_THEOS_RPM_PACKAGE_FILENAME)"$(ECHO_END)
	$(ECHO_NOTHING)rm -r "$(THEOS_OBJ_DIR)/$(THEOS_PACKAGE_ARCH)"$(ECHO_END)

# This variable is used in package.mk
after-package:: __THEOS_LAST_PACKAGE_FILENAME = $(_THEOS_RPM_PACKAGE_FILENAME)

else # _THEOS_RPM_CAN_PACKAGE == 0
internal-package::
	$(ERROR_BEGIN)"$(MAKE) package requires you to have a package.spec file in the project directory."$(ERROR_END)

endif # _THEOS_RPM_CAN_PACKAGE
endif # _THEOS_PACKAGE_FORMAT_LOADED
