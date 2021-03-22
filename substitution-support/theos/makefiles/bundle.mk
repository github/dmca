ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/bundle.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),bundle)
		include $(THEOS_MAKE_PATH)/instance/bundle.mk
	endif
endif
