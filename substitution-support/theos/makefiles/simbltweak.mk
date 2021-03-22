ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/simbltweak.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),simbltweak)
		include $(THEOS_MAKE_PATH)/instance/simbltweak.mk
	endif
endif
