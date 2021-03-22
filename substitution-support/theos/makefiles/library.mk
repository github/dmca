ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/library.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),library)
		include $(THEOS_MAKE_PATH)/instance/library.mk
	endif
endif
