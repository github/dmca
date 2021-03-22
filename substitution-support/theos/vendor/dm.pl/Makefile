include $(THEOS)/makefiles/common.mk
include $(THEOS_MAKE_PATH)/null.mk

dm.pl.1: dm.pl
	$(ECHO_COMPILING)pod2man $^ $@$(ECHO_END)

after-stage:: dm.pl.1
	$(ECHO_NOTHING)mkdir -p $(THEOS_STAGING_DIR)/usr/bin $(THEOS_STAGING_DIR)/usr/share/man/man1$(ECHO_END)
	$(ECHO_NOTHING)cp dm.pl $(THEOS_STAGING_DIR)/usr/bin/dm.pl$(ECHO_END)
	$(ECHO_NOTHING)ln -s dm.pl $(THEOS_STAGING_DIR)/usr/bin/dm$(ECHO_END)
	$(ECHO_NOTHING)cp dm.pl.1 $(THEOS_STAGING_DIR)/usr/share/man/man1/dm.pl.1$(ECHO_END)
