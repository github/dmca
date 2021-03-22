MY_TARGETS = ObfuscatedStrings ObfuscatedCalls DetectDebugger

.PHONY: all $(MY_TARGETS)
all: $(MY_TARGETS)

$(MY_TARGETS):
	@$(MAKE) -C $@ $(ARG)

clean: ARG=clean
clean: all
