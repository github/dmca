ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/null.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),null)
		include $(THEOS_MAKE_PATH)/instance/null.mk
	endif
endif
