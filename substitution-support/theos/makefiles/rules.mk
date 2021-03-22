ifeq ($(_THEOS_RULES_LOADED),)
_THEOS_RULES_LOADED := 1

ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/rules.mk
else
	include $(THEOS_MAKE_PATH)/instance/rules.mk
endif

ifeq ($(_THEOS_MAKE_PARALLEL_BUILDING), no)
.NOTPARALLEL:
else ifneq ($(_THEOS_MAKE_PARALLEL), yes)
.NOTPARALLEL:
endif

%.mm: %.l.mm
	$(THEOS_BIN_PATH)/logos.pl $(ALL_LOGOSFLAGS) $< > $@

%.mm: %.xmm
	$(THEOS_BIN_PATH)/logos.pl $(ALL_LOGOSFLAGS) $< > $@

%.mm: %.xm
	$(THEOS_BIN_PATH)/logos.pl $(ALL_LOGOSFLAGS) $< > $@

%.m: %.xm
	$(THEOS_BIN_PATH)/logos.pl $(ALL_LOGOSFLAGS) $< > $@

%.swift: %.xswift
	$(THEOS_BIN_PATH)/logos.pl $(ALL_LOGOSFLAGS) $< > $@

ifneq ($(THEOS_BUILD_DIR),.)
$(THEOS_BUILD_DIR):
	@mkdir -p $(THEOS_BUILD_DIR)
endif

$(THEOS_OBJ_DIR):
	@mkdir -p $@

$(THEOS_OBJ_DIR)/.stamp: $(THEOS_OBJ_DIR)
	@mkdir -p $(dir $@); touch $@

$(THEOS_OBJ_DIR)/%/.stamp: $(THEOS_OBJ_DIR)
	@mkdir -p $(dir $@); touch $@

Makefile: ;
$(_THEOS_RELATIVE_MAKE_PATH)%.mk: ;
$(THEOS_MAKE_PATH)/%.mk: ;
$(THEOS_MAKE_PATH)/master/%.mk: ;
$(THEOS_MAKE_PATH)/instance/%.mk: ;
$(THEOS_MAKE_PATH)/instance/shared/%.mk: ;
$(THEOS_MAKE_PATH)/platform/%.mk: ;
$(THEOS_MAKE_PATH)/targets/%.mk: ;
$(THEOS_MAKE_PATH)/targets/%/%.mk: ;

ifneq ($(THEOS_PACKAGE_DIR_NAME),)
$(THEOS_PACKAGE_DIR):
	@cd $(THEOS_BUILD_DIR); mkdir -p $(THEOS_PACKAGE_DIR_NAME)
endif

endif

# TODO MAKE A BUNCH OF THINGS PHONY
$(eval $(call __mod,rules.mk))
