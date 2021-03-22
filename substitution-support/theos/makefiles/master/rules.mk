__THEOS_RULES_MK_VERSION := 1k
ifneq ($(__THEOS_RULES_MK_VERSION),$(__THEOS_COMMON_MK_VERSION))
all::
	@echo "Theos version mismatch! common.mk [version $(or $(__THEOS_COMMON_MK_VERSION),0)] loaded in tandem with rules.mk [version $(or $(__THEOS_RULES_MK_VERSION),0)] Check that \$$\(THEOS\) is set properly!" >&2; exit 1
endif

# Determine whether we are on a modern enough version of make for us to enable parallel building.
# --output-sync was added in make 4.0; output is hard to read without it. Xcode includes make 3.81.
ifeq ($(THEOS_USE_PARALLEL_BUILDING),)
THEOS_USE_PARALLEL_BUILDING := $(call __simplify,THEOS_USE_PARALLEL_BUILDING,$(shell $(THEOS_BIN_PATH)/vercmp.pl $(MAKE_VERSION) gt 4.0))
endif

ifeq ($(call __theos_bool,$(THEOS_USE_PARALLEL_BUILDING)),$(_THEOS_TRUE))
# If jobs haven’t already been specified, and we know how to get the number of logical cores on this
# platform, set jobs to the logical core count (CPU cores multiplied by threads per core).
ifneq ($(_THEOS_PLATFORM_GET_LOGICAL_CORES),)
ifeq ($(findstring --jobserver-auth=,$(MAKEFLAGS)),)
MAKEFLAGS += -j$(shell $(_THEOS_PLATFORM_GET_LOGICAL_CORES)) -Otarget
endif
endif
endif

.PHONY: all before-all internal-all after-all \
	clean before-clean internal-clean after-clean \
	clean-packages before-clean-packages internal-clean-packages after-clean-packages \
	update-theos
ifeq ($(THEOS_BUILD_DIR),.)
all:: $(_THEOS_BUILD_SESSION_FILE) before-all internal-all after-all
else
all:: $(THEOS_BUILD_DIR) $(_THEOS_BUILD_SESSION_FILE) before-all internal-all after-all
endif

clean:: before-clean internal-clean after-clean

do:: all package install

before-all::
# If the sysroot is set but doesn’t exist, bail out.
ifneq ($(SYSROOT),)
ifneq ($(call __exists,$(SYSROOT)),$(_THEOS_TRUE))
	$(ERROR_BEGIN) "Your current SYSROOT, “$(SYSROOT)”, appears to be missing." $(ERROR_END)
endif
endif

# If a vendored path is missing, bail out.
ifneq ($(call __exists,$(THEOS_VENDOR_INCLUDE_PATH)/.git)$(call __exists,$(THEOS_VENDOR_LIBRARY_PATH)/.git),$(_THEOS_TRUE)$(_THEOS_TRUE))
	$(ERROR_BEGIN) "The vendor/include and/or vendor/lib directories are missing. Please run \`$(THEOS)/bin/update-theos\`. More information: https://github.com/theos/theos/wiki/Installation." $(ERROR_END)
endif

ifeq ($(call __exists,$(THEOS_LEGACY_PACKAGE_DIR)),$(_THEOS_TRUE))
ifneq ($(call __exists,$(THEOS_PACKAGE_DIR)),$(_THEOS_TRUE))
	@$(PRINT_FORMAT) "The \"debs\" directory has been renamed to \"packages\". Moving it." >&2
	$(ECHO_NOTHING)mv "$(THEOS_LEGACY_PACKAGE_DIR)" "$(THEOS_PACKAGE_DIR)"$(ECHO_END)
endif
endif

internal-all::

after-all::

before-clean::

internal-clean::
	$(ECHO_CLEANING)rm -rf "$(subst $(_THEOS_OBJ_DIR_EXTENSION),,$(THEOS_OBJ_DIR))"$(ECHO_END)

ifeq ($(call __exists,$(_THEOS_BUILD_SESSION_FILE)),$(_THEOS_TRUE))
	$(ECHO_NOTHING)rm "$(_THEOS_BUILD_SESSION_FILE)"$(ECHO_END)
	$(ECHO_NOTHING)touch "$(_THEOS_BUILD_SESSION_FILE)"$(ECHO_END)
endif

ifeq ($(MAKELEVEL),0)
	$(ECHO_NOTHING)rm -rf "$(THEOS_STAGING_DIR)"$(ECHO_END)
endif

after-clean::

ifeq ($(MAKELEVEL),0)
ifneq ($(THEOS_BUILD_DIR),.)
_THEOS_ABSOLUTE_BUILD_DIR = $(call __clean_pwd,$(THEOS_BUILD_DIR))
else
_THEOS_ABSOLUTE_BUILD_DIR = .
endif
else
_THEOS_ABSOLUTE_BUILD_DIR = $(strip $(THEOS_BUILD_DIR))
endif

clean-packages:: before-clean-packages internal-clean-packages after-clean-packages

before-clean-packages::

internal-clean-packages::
	$(ECHO_NOTHING)rm -rf $(THEOS_PACKAGE_DIR)/$(THEOS_PACKAGE_NAME)_*-*_$(THEOS_PACKAGE_ARCH).deb$(ECHO_END)
	$(ECHO_NOTHING)rm -rf $(THEOS_PACKAGE_DIR)/$(THEOS_PACKAGE_NAME)-*-*.$(THEOS_PACKAGE_ARCH).rpm$(ECHO_END)

after-clean-packages::

$(_THEOS_BUILD_SESSION_FILE):
	@mkdir -p $(_THEOS_LOCAL_DATA_DIR)

ifeq ($(call __exists,$(_THEOS_BUILD_SESSION_FILE)),$(_THEOS_FALSE))
	@touch $(_THEOS_BUILD_SESSION_FILE)
endif

.PRECIOUS: %.variables %.subprojects

%.variables: _INSTANCE = $(basename $(basename $*))
%.variables: _OPERATION = $(subst .,,$(suffix $(basename $*)))
%.variables: _TYPE = $(subst -,_,$(subst .,,$(suffix $*)))
%.variables: __SUBPROJECTS = $(strip $(call __schema_var_all,$(_INSTANCE)_,SUBPROJECTS))
%.variables:
	+@ \
abs_build_dir=$(_THEOS_ABSOLUTE_BUILD_DIR); \
if [[ "$(__SUBPROJECTS)" != "" ]]; then \
  $(PRINT_FORMAT_MAKING) "Making $(_OPERATION) in subprojects of $(_TYPE) $(_INSTANCE)"; \
  for d in $(__SUBPROJECTS); do \
    d="$${d%:*}"; \
    if [[ "$${abs_build_dir}" = "." ]]; then \
      lbuilddir="."; \
    else \
      lbuilddir="$${abs_build_dir}/$$d"; \
    fi; \
    if $(MAKE) -C $$d -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $(_OPERATION) \
        THEOS_BUILD_DIR="$$lbuilddir" \
       ; then\
       :; \
    else exit $$?; \
    fi; \
  done; \
 fi; \
$(PRINT_FORMAT_MAKING) "Making $(_OPERATION) for $(_TYPE) $(_INSTANCE)"; \
$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) \
	internal-$(_TYPE)-$(_OPERATION) \
	_THEOS_CURRENT_TYPE="$(_TYPE)" \
	THEOS_CURRENT_INSTANCE="$(_INSTANCE)" \
	_THEOS_CURRENT_OPERATION="$(_OPERATION)" \
	THEOS_BUILD_DIR="$(_THEOS_ABSOLUTE_BUILD_DIR)"

%.subprojects: _INSTANCE = $(basename $(basename $*))
%.subprojects: _OPERATION = $(subst .,,$(suffix $(basename $*)))
%.subprojects: _TYPE = $(subst -,_,$(subst .,,$(suffix $*)))
%.subprojects: __SUBPROJECTS = $(strip $(call __schema_var_all,$(_INSTANCE)_,SUBPROJECTS))
%.subprojects:
	+@ \
abs_build_dir=$(_THEOS_ABSOLUTE_BUILD_DIR); \
if [[ "$(__SUBPROJECTS)" != "" ]]; then \
  $(PRINT_FORMAT_MAKING) "Making $(_OPERATION) in subprojects of $(_TYPE) $(_INSTANCE)"; \
  for d in $(__SUBPROJECTS); do \
    d="$${d%:*}"; \
    if [[ "$${abs_build_dir}" = "." ]]; then \
      lbuilddir="."; \
    else \
      lbuilddir="$${abs_build_dir}/$$d"; \
    fi; \
    if $(MAKE) -C $$d -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $(_OPERATION) \
        THEOS_BUILD_DIR="$$lbuilddir" \
       ; then\
       :; \
    else exit $$?; \
    fi; \
  done; \
 fi

update-theos::
	@$(PRINT_FORMAT_MAKING) "Updating Theos"
	$(ECHO_NOTHING)$(THEOS_BIN_PATH)/update-theos$(ECHO_END)

troubleshoot::
	@$(PRINT_FORMAT) "Be sure to check the troubleshooting page at https://github.com/theos/theos/wiki/Troubleshooting first."
	@$(PRINT_FORMAT) "For support with build errors, ask on IRC: http://iphonedevwiki.net/index.php/IRC. If you think you've found a bug in Theos, check the issue tracker at https://github.com/theos/theos/issues."
	@echo

ifeq ($(call __executable,ghost),$(_THEOS_TRUE))
	@$(PRINT_FORMAT) "Creating a Ghostbin containing the output of \`make clean all messages=yes\`…"
	+$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) --no-print-directory --no-keep-going clean all messages=yes COLOR=yes 2>&1 | ghost -x 2w - ansi
else
	$(ERROR_BEGIN) "You don't have ghost installed. For more information, refer to https://github.com/theos/theos/wiki/Installation#prerequisites." $(ERROR_END)
endif

$(eval $(call __mod,master/rules.mk))

ifeq ($(_THEOS_TOP_INVOCATION_DONE),)
export _THEOS_TOP_INVOCATION_DONE = 1
endif
