ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/application.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),application)
		include $(THEOS_MAKE_PATH)/instance/application.mk
	endif
endif
