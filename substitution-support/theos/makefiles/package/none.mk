ifeq ($(_THEOS_PACKAGE_FORMAT_LOADED),)
_THEOS_PACKAGE_FORMAT_LOADED := 1

# This package format does nothing - it simply relies upon the internal-package rule in package.mk to do nothing for it.
endif # _THEOS_PACKAGE_FORMAT_LOADED
