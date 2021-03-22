ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/subproject.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),subproject)
		include $(THEOS_MAKE_PATH)/instance/subproject.mk
	endif
endif
