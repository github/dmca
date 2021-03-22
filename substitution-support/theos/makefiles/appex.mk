ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/appex.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),appex)
		include $(THEOS_MAKE_PATH)/instance/appex.mk
	endif
endif
