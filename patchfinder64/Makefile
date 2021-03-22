ifeq ($(OS),Windows_NT) 
	detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

all: kerneldec patchfinder64

.PHONY: patchfinder64 kerneldec

bin:
	@mkdir bin

patchfinder64: bin/patchfinder64

bin/patchfinder64: patchfinder64.c patchfinder64.h | bin
ifeq ($(detected_OS),Darwin)
		$(CC) -g -O2 -DHAVE_MAIN patchfinder64.c -o ./bin/patchfinder64
else
		$(CC) -g -O2 -DHAVE_MAIN -DNOT_DARWIN patchfinder64.c -o ./bin/patchfinder64
endif

kerneldec:
	@make -C kerneldec

clean:
	rm -f bin/patchfinder64
