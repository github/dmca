ifeq ($(_THEOS_AGGREGATE_MK_LOADED),)
	_THEOS_AGGREGATE_MK_LOADED := 1

	ifeq ($(THEOS_CURRENT_INSTANCE),)
		include $(THEOS_MAKE_PATH)/master/aggregate.mk
	endif
endif
