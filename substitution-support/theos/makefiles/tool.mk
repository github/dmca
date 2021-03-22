ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/tool.mk
else
	ifeq ($(_THEOS_CURRENT_TYPE),tool)
		include $(THEOS_MAKE_PATH)/instance/tool.mk
	endif
endif
