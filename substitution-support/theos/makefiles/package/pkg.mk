ifeq ($(_THEOS_PACKAGE_FORMAT_LOADED),)
_THEOS_PACKAGE_FORMAT_LOADED := 1

_THEOS_PKG_PACKAGE_CONTROL_PATH := $(THEOS_PROJECT_DIR)/control
_THEOS_PKG_CAN_PACKAGE := $(if $(_THEOS_PKG_PACKAGE_CONTROL_PATH),$(_THEOS_TRUE),$(_THEOS_FALSE))
_THEOS_PACKAGE_INC_VERSION_PREFIX := -
_THEOS_PACKAGE_EXTRA_VERSION_PREFIX := +

_THEOS_PKG_HAS_PKGBUILD := $(call __executable,pkgbuild)
ifneq ($(_THEOS_PKG_HAS_PKGBUILD),$(_THEOS_TRUE))
internal-package-check::
	$(ERROR_BEGIN)"$(MAKE) package requires pkgbuild."$(ERROR_END)
endif

ifeq ($(_THEOS_PKG_CAN_PACKAGE),$(_THEOS_TRUE)) # Control file found (or layout directory found.)
THEOS_PACKAGE_NAME := $(shell grep -i "^Package:" "$(_THEOS_PKG_PACKAGE_CONTROL_PATH)" | cut -d' ' -f2-)
THEOS_PACKAGE_BASE_VERSION := $(shell grep -i "^Version:" "$(_THEOS_PKG_PACKAGE_CONTROL_PATH)" | cut -d' ' -f2-)

_THEOS_PKG_PACKAGE_FILENAME = $(THEOS_PACKAGE_DIR)/$(THEOS_PACKAGE_NAME)_$(_THEOS_INTERNAL_PACKAGE_VERSION).pkg
internal-package::
	$(ECHO_NOTHING)COPYFILE_DISABLE=1 $(FAKEROOT) -r pkgbuild --root "$(THEOS_STAGING_DIR)" --identifier "$(THEOS_PACKAGE_NAME)" --version "$(_THEOS_INTERNAL_PACKAGE_VERSION)" --ownership recommended "$(_THEOS_PKG_PACKAGE_FILENAME)" $(STDERR_NULL_REDIRECT)$(ECHO_END)

# This variable is used in package.mk
after-package:: __THEOS_LAST_PACKAGE_FILENAME = $(_THEOS_PKG_PACKAGE_FILENAME)

else # _THEOS_PKG_CAN_PACKAGE == 0
internal-package::
	$(ERROR_BEGIN)"$(MAKE) package requires you to have a control file in the project root describing the package."$(ERROR_END)

endif # _THEOS_PKG_CAN_PACKAGE
endif # _THEOS_PACKAGE_FORMAT_LOADED
