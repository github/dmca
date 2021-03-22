ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/framework.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),framework)
		include $(THEOS_MAKE_PATH)/instance/framework.mk
	endif
endif
